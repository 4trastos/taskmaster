#ifndef PROCESSMANAGER_HPP
#define PROCESSMANAGER_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <csignal>
#include <ctime>

class CProgram;

enum class ProcessState {
    STOPPED,
    STARTING,
    RUNNING,
    BACKOFF,
    STOPPING,
    EXITED,
    FATAL,
    STOPPED_PERM,
    UNKNOWN
};

struct ProcessInfo {
    pid_t pid;
    int instance_id;
    time_t start_time;
    int restart_count;
    int exit_code;
    int stdout_fd;
    int stderr_fd;
    ProcessState state;

    ProcessInfo()
        : pid(0)
        , instance_id(0)
        , start_time(0)
        , restart_count(0)
        , exit_code(0)
        , stdout_fd(-1)
        , stderr_fd(-1)
        , state(ProcessState::STOPPED)
    {}
};

class CProcessManager {
private:
    std::map<std::string, std::vector<ProcessInfo>> m_processes;

public:
    CProcessManager();
    ~CProcessManager();

    // Inicialización y arranque
    void InitializeProgram(const CProgram& program);
    void StartAutoStartPrograms(const std::vector<CProgram>& programs);
    
    // Control de procesos
    bool LaunchProcess(const CProgram& program, int instance = 0);
    bool StopProcess(pid_t pid, int signal, int status);
    bool RestartProcess(const CProgram& program, int instance = 0);
    
    // Monitoreo
    void MonitorProcesses(const std::vector<CProgram>& programs);
    void HandleChildStatusChange(const std::vector<CProgram>& programs);
    
    // Políticas
    void ApplyRestartPolicy(const CProgram& program, int instance, int status);
    void ApplyStopPolicy(pid_t pid, int signal, int status);
    
    // Verificaciones
    bool IsExpectedExit(int exit_status, const std::vector<int>& exitcodes) const;
    ProcessState GetProcessState(const std::string& name, int instance = 0) const;
    void SetProcessState(const std::string& name, int instance, ProcessState state);
    
    // Getters
    const ProcessInfo* GetProcessInfo(const std::string& name, int instance = 0) const;
    ProcessInfo* GetProcessInfo(const std::string& name, int instance = 0);
    
    // Limpieza
    void AbortRestart(const CProgram& program, int instance);
    void Cleanup();

private:
    std::string GetProcessKey(const std::string& name, int instance) const;
    void SetupRedirections(const CProgram& program, ProcessInfo& info);
};

#endif // PROCESSMANAGER_HPP