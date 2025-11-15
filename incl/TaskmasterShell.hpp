#ifndef TASKMASTERSHELL_HPP
#define TASKMASTERSHELL_HPP

#include <string>
#include <vector>
#include <iostream>

class CProgram;
class CProcessManager;

class CTaskmasterShell {
private:
    CProcessManager* m_process_manager;
    std::vector<CProgram>* m_programs;
    bool m_running;

public:
    CTaskmasterShell(CProcessManager* pm, std::vector<CProgram>* programs);
    ~CTaskmasterShell();
    
    // Control del shell
    void Start();
    void Stop();
    bool IsRunning() const { return m_running; }
    
    // Procesamiento de comandos
    bool ProcessCommand(const std::string& command);
    
    // Loop principal
    int PromptLoop();
    
    // Verificación de entrada disponible
    static bool UserInputReady();

private:
    // Comandos disponibles
    bool CmdStart(const std::vector<std::string>& args);
    bool CmdStop(const std::vector<std::string>& args);
    bool CmdRestart(const std::vector<std::string>& args);
    bool CmdStatus(const std::vector<std::string>& args);
    bool CmdExit(const std::vector<std::string>& args);
    bool CmdHelp(const std::vector<std::string>& args);
    bool CmdReload(const std::vector<std::string>& args);
    
    // Utilidades
    std::vector<std::string> ParseCommand(const std::string& command);
    std::string TrimString(const std::string& str);
    void ShowPrompt();
    void DisplayHelp();
};

#endif // TASKMASTERSHELL_HPP