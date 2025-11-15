#include "ProcessManager.hpp"
#include "Program.hpp"
#include "Logger.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>

extern char **environ;

CProcessManager::CProcessManager() {
}

CProcessManager::~CProcessManager() {
    Cleanup();
}

std::string CProcessManager::GetProcessKey(const std::string& name, int instance) const {
    return name + "_" + std::to_string(instance);
}

void CProcessManager::InitializeProgram(const CProgram& program) {
    int numprocs = program.GetConfig().numprocs;
    
    for (int i = 0; i < numprocs; ++i) {
        ProcessInfo info;
        info.instance_id = i;
        info.state = ProcessState::STOPPED;
        
        std::string key = GetProcessKey(program.GetName(), i);
        m_processes[key] = {info};
    }
}

void CProcessManager::StartAutoStartPrograms(const std::vector<CProgram>& programs) {
    for (const auto& program : programs) {
        if (program.GetConfig().autostart) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                LaunchProcess(program, i);
            }
        }
    }
}

void CProcessManager::SetupRedirections(const CProgram& program, ProcessInfo& info) {
    const auto& cfg = program.GetConfig();
    
    // Redirección stdout
    if (!cfg.stdout_path.empty()) {
        info.stdout_fd = open(cfg.stdout_path.c_str(), 
                             O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (info.stdout_fd != -1) {
            dup2(info.stdout_fd, STDOUT_FILENO);
            close(info.stdout_fd);
        }
    }
    
    // Redirección stderr
    if (!cfg.stderr_path.empty()) {
        info.stderr_fd = open(cfg.stderr_path.c_str(), 
                             O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (info.stderr_fd != -1) {
            dup2(info.stderr_fd, STDERR_FILENO);
            close(info.stderr_fd);
        }
    }
}

bool CProcessManager::LaunchProcess(const CProgram& program, int instance) {
    std::string key = GetProcessKey(program.GetName(), instance);
    
    if (m_processes.find(key) == m_processes.end()) {
        return false;
    }
    
    ProcessInfo& info = m_processes[key][0];
    const auto& cfg = program.GetConfig();
    
    pid_t pid = fork();
    
    if (pid == -1) {
        CLogger::Error("Failed to fork process for " + program.GetName() + 
                      ": " + std::string(strerror(errno)));
        return false;
    }
    
    if (pid == 0) {
        // Proceso hijo
        
        // Cambiar directorio de trabajo si está especificado
        if (!cfg.workingdir.empty()) {
            if (chdir(cfg.workingdir.c_str()) == -1) {
                std::cerr << "Error: chdir failed: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        
        // Configurar umask si está especificado
        if (!cfg.umask.empty()) {
            mode_t mask = std::stoi(cfg.umask, nullptr, 8);
            umask(mask);
        }
        
        // Redirecciones
        SetupRedirections(program, info);
        
        // Preparar argumentos para execve
        std::vector<char*> argv_exec;
        argv_exec.push_back(const_cast<char*>(cfg.cmd.c_str()));
        argv_exec.push_back(nullptr);
        
        // Preparar variables de entorno
        std::vector<std::string> env_strings;
        std::vector<char*> envp_exec;
        
        if (!program.GetEnv().empty()) {
            for (const auto& pair : program.GetEnv()) {
                env_strings.push_back(pair.first + "=" + pair.second);
            }
            for (auto& str : env_strings) {
                envp_exec.push_back(const_cast<char*>(str.c_str()));
            }
            envp_exec.push_back(nullptr);
            
            execve(cfg.cmd.c_str(), argv_exec.data(), envp_exec.data());
        } else {
            execve(cfg.cmd.c_str(), argv_exec.data(), environ);
        }
        
        // Si llegamos aquí, execve falló
        std::cerr << "Error: execve failed for " << program.GetName() 
                  << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Proceso padre
    info.pid = pid;
    info.start_time = time(nullptr);
    info.state = ProcessState::RUNNING;
    info.restart_count = 0;
    
    CLogger::Info("Process '" + program.GetName() + "' [" + 
                 std::to_string(instance) + "] started with PID: " + 
                 std::to_string(pid));
    
    return true;
}

bool CProcessManager::StopProcess(pid_t pid, int signal, int status) {
    if (pid <= 0) {
        return false;
    }
    
    ApplyStopPolicy(pid, signal, status);
    
    if (kill(pid, signal) == -1) {
        if (errno == ESRCH) {
            // El proceso ya no existe
            return true;
        }
        CLogger::Error("Failed to send signal " + std::to_string(signal) + 
                      " to PID " + std::to_string(pid) + ": " + 
                      std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

void CProcessManager::ApplyStopPolicy(pid_t pid, int signal, int status) {
    std::string msg;
    
    if (WIFEXITED(status)) {
        msg = "Process PID " + std::to_string(pid) + 
              " exited with code: " + std::to_string(WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        msg = "Process PID " + std::to_string(pid) + 
              " terminated by signal: " + std::to_string(WTERMSIG(status));
    } else {
        msg = "Process PID " + std::to_string(pid) + 
              " terminated with unknown status";
    }
    
    CLogger::Info(msg);
    CLogger::Info("Stop signal configured: " + std::to_string(signal));
}

bool CProcessManager::IsExpectedExit(int exit_status, 
                                     const std::vector<int>& exitcodes) const {
    if (exit_status < 0) {
        return false;
    }
    
    for (int code : exitcodes) {
        if (code == exit_status) {
            return true;
        }
    }
    
    return false;
}

void CProcessManager::ApplyRestartPolicy(const CProgram& program, 
                                        int instance, int status) {
    std::string key = GetProcessKey(program.GetName(), instance);
    
    if (m_processes.find(key) == m_processes.end()) {
        return;
    }
    
    ProcessInfo& info = m_processes[key][0];
    const auto& cfg = program.GetConfig();
    
    // Verificar si el autorestart es NEVER
    if (cfg.autorestart == CProgram::Autorestart::NEVER) {
        info.state = ProcessState::STOPPED;
        return;
    }
    
    bool expected_exit = false;
    
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        expected_exit = IsExpectedExit(exit_code, cfg.exitcodes);
    }
    
    // Aplicar política según autorestart
    if (cfg.autorestart == CProgram::Autorestart::ALWAYS) {
        info.state = ProcessState::STARTING;
    } else if (cfg.autorestart == CProgram::Autorestart::UNEXPECTED) {
        if (!expected_exit) {
            info.state = ProcessState::STARTING;
        } else {
            info.state = ProcessState::STOPPED;
        }
    }
    
    // Verificar límite de reintentos
    if (info.state == ProcessState::STARTING) {
        if (info.restart_count < cfg.startretries) {
            info.restart_count++;
        } else {
            info.state = ProcessState::STOPPED_PERM;
            CLogger::Warning("Process '" + program.GetName() + 
                           "' reached max restart attempts");
        }
    }
}

void CProcessManager::MonitorProcesses(const std::vector<CProgram>& programs) {
    for (const auto& program : programs) {
        int numprocs = program.GetConfig().numprocs;
        
        for (int i = 0; i < numprocs; ++i) {
            std::string key = GetProcessKey(program.GetName(), i);
            
            if (m_processes.find(key) == m_processes.end()) {
                continue;
            }
            
            ProcessInfo& info = m_processes[key][0];
            
            // Lógica de arranque
            if (info.state == ProcessState::STARTING && info.pid == 0) {
                LaunchProcess(program, i);
            }
            
            // Lógica de parada
            if (info.state == ProcessState::STOPPING && info.pid > 0) {
                int signal = SIGTERM; // Default
                if (!program.GetConfig().stopsignal.empty()) {
                    // Aquí podrías parsear la señal desde el string
                    // Por ahora usamos SIGTERM por defecto
                }
                StopProcess(info.pid, signal, 0);
            }
        }
    }
}

void CProcessManager::HandleChildStatusChange(const std::vector<CProgram>& programs) {
    pid_t pid;
    int status;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Buscar el proceso en nuestro mapa
        for (const auto& program : programs) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                std::string key = GetProcessKey(program.GetName(), i);
                
                if (m_processes.find(key) != m_processes.end()) {
                    ProcessInfo& info = m_processes[key][0];
                    
                    if (info.pid == pid) {
                        CLogger::Warning("Process '" + program.GetName() + 
                                       "' [" + std::to_string(i) + 
                                       "] (PID " + std::to_string(pid) + 
                                       ") has terminated");
                        
                        // Limpieza
                        info.pid = 0;
                        info.state = ProcessState::STOPPED;
                        
                        // Aplicar política de reinicio
                        ApplyRestartPolicy(program, i, status);
                        
                        goto next_pid;
                    }
                }
            }
        }
        
next_pid:
        continue;
    }
}

ProcessState CProcessManager::GetProcessState(const std::string& name, 
                                              int instance) const {
    std::string key = GetProcessKey(name, instance);
    
    auto it = m_processes.find(key);
    if (it != m_processes.end()) {
        return it->second[0].state;
    }
    
    return ProcessState::UNKNOWN;
}

void CProcessManager::SetProcessState(const std::string& name, 
                                      int instance, ProcessState state) {
    std::string key = GetProcessKey(name, instance);
    
    auto it = m_processes.find(key);
    if (it != m_processes.end()) {
        it->second[0].state = state;
    }
}

const ProcessInfo* CProcessManager::GetProcessInfo(const std::string& name, 
                                                   int instance) const {
    std::string key = GetProcessKey(name, instance);
    
    auto it = m_processes.find(key);
    if (it != m_processes.end()) {
        return &it->second[0];
    }
    
    return nullptr;
}

ProcessInfo* CProcessManager::GetProcessInfo(const std::string& name, 
                                             int instance) {
    std::string key = GetProcessKey(name, instance);
    
    auto it = m_processes.find(key);
    if (it != m_processes.end()) {
        return &it->second[0];
    }
    
    return nullptr;
}

void CProcessManager::AbortRestart(const CProgram& program, int instance) {
    std::string key = GetProcessKey(program.GetName(), instance);
    
    auto it = m_processes.find(key);
    if (it != m_processes.end()) {
        it->second[0].state = ProcessState::FATAL;
        CLogger::Error("Restart aborted for '" + program.GetName() + "'");
    }
}

void CProcessManager::Cleanup() {
    // Detener todos los procesos en ejecución
    for (auto& pair : m_processes) {
        ProcessInfo& info = pair.second[0];
        if (info.pid > 0) {
            kill(info.pid, SIGTERM);
        }
    }
    
    m_processes.clear();
}