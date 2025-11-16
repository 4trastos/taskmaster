#include "TaskmasterShell.hpp"
#include "ProcessManager.hpp"
#include "Program.hpp"
#include "Logger.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/select.h>
#include <unistd.h>

#define PROMPT "TaskMaster> "

CTaskmasterShell::CTaskmasterShell(CProcessManager* pm, 
                                   std::vector<CProgram>* programs)
    : m_process_manager(pm)
    , m_programs(programs)
    , m_running(false)
{
}

CTaskmasterShell::~CTaskmasterShell() {
}

void CTaskmasterShell::Start() {
    m_running = true;
    CLogger::Info("TaskMaster shell started");
}

void CTaskmasterShell::Stop() {
    m_running = false;
    CLogger::Info("TaskMaster shell stopped");
}

std::string CTaskmasterShell::TrimString(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> CTaskmasterShell::ParseCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::istringstream iss(command);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool CTaskmasterShell::ProcessCommand(const std::string& command) {
    if (command.empty()) {
        return true;
    }
    
    std::vector<std::string> tokens = ParseCommand(command);
    
    if (tokens.empty()) {
        return true;
    }
    
    std::string cmd = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    // Convertir a minúsculas para comparación
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "start") {
        return CmdStart(args);
    } else if (cmd == "stop") {
        return CmdStop(args);
    } else if (cmd == "restart") {
        return CmdRestart(args);
    } else if (cmd == "status") {
        return CmdStatus(args);
    } else if (cmd == "exit" || cmd == "quit") {
        return CmdExit(args);
    } else if (cmd == "help") {
        return CmdHelp(args);
    } else if (cmd == "reload") {
        return CmdReload(args);
    } else {
        std::cout << "❌ Unknown command: " << cmd << std::endl;
        std::cout << "Type 'help' for available commands" << std::endl;
        return true;
    }
}

bool CTaskmasterShell::CmdStart(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: start <program_name>" << std::endl;
        return true;
    }
    
    std::string program_name = args[0];
    
    // Buscar el programa
    for (const auto& program : *m_programs) {
        if (program.GetName() == program_name) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                ProcessInfo* info = m_process_manager->GetProcessInfo(program_name, i);
                if (info) {
                    info->restart_count = 0;
                    CLogger::Info("Manual start of '" + program_name + 
                                 "' [" + std::to_string(i) + 
                                 "], restart counter reset to 0");
                }
                
                m_process_manager->SetProcessState(program_name, i, 
                                                   ProcessState::STARTING);
            }
            
            std::cout << "✅ Starting program: " << program_name << std::endl;
            return true;
        }
    }
    
    std::cout << "❌ Program not found: " << program_name << std::endl;
    return true;
}

bool CTaskmasterShell::CmdStop(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: stop <program_name>" << std::endl;
        return true;
    }
    
    std::string program_name = args[0];
    
    // Buscar el programa
    for (const auto& program : *m_programs) {
        if (program.GetName() == program_name) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                m_process_manager->SetProcessState(program_name, i, 
                                                   ProcessState::STOPPING);
            }
            
            std::cout << "✅ Stopping program: " << program_name << std::endl;
            return true;
        }
    }
    
    std::cout << "❌ Program not found: " << program_name << std::endl;
    return true;
}

bool CTaskmasterShell::CmdRestart(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: restart <program_name>" << std::endl;
        return true;
    }
    
    std::string program_name = args[0];
    
    // Buscar el programa
    for (const auto& program : *m_programs) {
        if (program.GetName() == program_name) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                ProcessInfo* info = m_process_manager->GetProcessInfo(program_name, i);
                if (info) {
                    info->restart_count = 0;
                }
            }
            
            // Primero parar
            for (int i = 0; i < numprocs; ++i) {
                m_process_manager->SetProcessState(program_name, i, 
                                                   ProcessState::STOPPING);
            }
            
            // Luego arrancar
            for (int i = 0; i < numprocs; ++i) {
                m_process_manager->SetProcessState(program_name, i, 
                                                   ProcessState::STARTING);
            }
            
            std::cout << "✅ Restarting program: " << program_name << std::endl;
            return true;
        }
    }
    
    std::cout << "❌ Program not found: " << program_name << std::endl;
    return true;
}

bool CTaskmasterShell::CmdStatus(const std::vector<std::string>& args) {
    if (args.empty()) {
        // Mostrar estado de todos los programas
        std::cout << "\n=== Process Status ===" << std::endl;
        
        for (const auto& program : *m_programs) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                const ProcessInfo* info = 
                    m_process_manager->GetProcessInfo(program.GetName(), i);
                
                if (info) {
                    std::cout << program.GetName() << "[" << i << "]: ";
                    
                    switch (info->state) {
                        case ProcessState::STOPPED:
                            std::cout << "STOPPED";
                            break;
                        case ProcessState::STARTING:
                            std::cout << "STARTING";
                            break;
                        case ProcessState::RUNNING:
                            std::cout << "RUNNING (PID: " << info->pid << ")";
                            break;
                        case ProcessState::STOPPING:
                            std::cout << "STOPPING";
                            break;
                        case ProcessState::EXITED:
                            std::cout << "EXITED";
                            break;
                        case ProcessState::FATAL:
                            std::cout << "FATAL";
                            break;
                        case ProcessState::STOPPED_PERM:
                            std::cout << "STOPPED (permanent)";
                            break;
                        default:
                            std::cout << "UNKNOWN";
                    }
                    
                    std::cout << std::endl;
                }
            }
        }
        
        std::cout << std::endl;
    } else {
        // Mostrar estado de un programa específico
        std::string program_name = args[0];
        bool found = false;
        
        for (const auto& program : *m_programs) {
            if (program.GetName() == program_name) {
                found = true;
                int numprocs = program.GetConfig().numprocs;
                
                std::cout << "\n=== Status: " << program_name << " ===" << std::endl;
                
                for (int i = 0; i < numprocs; ++i) {
                    const ProcessInfo* info = 
                        m_process_manager->GetProcessInfo(program_name, i);
                    
                    if (info) {
                        std::cout << "Instance " << i << ": ";
                        std::cout << "PID=" << info->pid << ", ";
                        std::cout << "Restarts=" << info->restart_count;
                        std::cout << std::endl;
                    }
                }
                
                std::cout << std::endl;
                break;
            }
        }
        
        if (!found) {
            std::cout << "❌ Program not found: " << program_name << std::endl;
        }
    }
    
    return true;
}

bool CTaskmasterShell::CmdExit(const std::vector<std::string>& args) {
    (void)args;
    
    std::cout << "Shutting down TaskMaster..." << std::endl;
    m_running = false;
    
    // Detener todos los procesos
    for (const auto& program : *m_programs) {
        int numprocs = program.GetConfig().numprocs;
        
        for (int i = 0; i < numprocs; ++i) {
            m_process_manager->SetProcessState(program.GetName(), i, 
                                               ProcessState::STOPPING);
        }
    }
    
    return false; // Señal para salir del loop
}

bool CTaskmasterShell::CmdHelp(const std::vector<std::string>& args) {
    (void)args;
    DisplayHelp();
    return true;
}

bool CTaskmasterShell::CmdReload(const std::vector<std::string>& args) {
    (void)args;
    std::cout << "⚠️  Configuration reload not yet implemented" << std::endl;
    return true;
}

void CTaskmasterShell::DisplayHelp() {
    std::cout << "\n=== TaskMaster Commands ===" << std::endl;
    std::cout << "  start <program>    - Start a program" << std::endl;
    std::cout << "  stop <program>     - Stop a program" << std::endl;
    std::cout << "  restart <program>  - Restart a program" << std::endl;
    std::cout << "  status [program]   - Show program status" << std::endl;
    std::cout << "  reload             - Reload configuration" << std::endl;
    std::cout << "  help               - Show this help" << std::endl;
    std::cout << "  exit/quit          - Exit TaskMaster" << std::endl;
    std::cout << std::endl;
}

bool CTaskmasterShell::UserInputReady() {
    struct timeval tv = {0, 50000}; // 50ms timeout
    fd_set fds;
    
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    
    return (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0);
}

int CTaskmasterShell::PromptLoop() {
    // Mostrar prompt
    std::cout << PROMPT << std::flush;
    
    // Leer línea
    std::string line;
    if (!std::getline(std::cin, line)) {
        // EOF (Ctrl+D)
        return -1;
    }
    
    std::string command = TrimString(line);
    
    if (!command.empty()) {
        if (!ProcessCommand(command)) {
            return -1; // Exit requested
        }
    }
    
    return 0;
}