#include "ConfigParser.hpp"
#include "ProcessManager.hpp"
#include "SignalHandler.hpp"
#include "TaskmasterShell.hpp"
#include "Logger.hpp"
#include <iostream>
#include <unistd.h>

#define PROMPT "TaskMaster> "

class CTaskmaster {
private:
    CConfigParser* m_config_parser;
    CProcessManager* m_process_manager;
    CTaskmasterShell* m_shell;
    std::vector<CProgram> m_programs;
    bool m_running;

public:
    CTaskmaster(const std::string& config_file)
        : m_config_parser(nullptr)
        , m_process_manager(nullptr)
        , m_shell(nullptr)
        , m_running(false)
    {
        // Inicializar logger
        CLogger::Init("/tmp/taskmaster.log", LogLevel::INFO);
        CLogger::Info("=== TaskMaster Starting ===");
        
        // Cargar configuración
        m_config_parser = new CConfigParser(config_file);
        m_programs = m_config_parser->GetPrograms();
        
        if (m_programs.empty()) {
            CLogger::Error("No programs found in configuration file");
            throw std::runtime_error("Invalid configuration");
        }
        
        CLogger::Info("Loaded " + std::to_string(m_programs.size()) + 
                     " programs from configuration");
        
        // Inicializar manejador de procesos
        m_process_manager = new CProcessManager();
        
        // Inicializar programas en el process manager
        for (const auto& program : m_programs) {
            m_process_manager->InitializeProgram(program);
        }
        
        // Inicializar shell
        m_shell = new CTaskmasterShell(m_process_manager, &m_programs);
        
        // Inicializar manejadores de señales
        CSignalHandler::Init();
    }
    
    ~CTaskmaster() {
        CLogger::Info("=== TaskMaster Shutting Down ===");
        
        if (m_shell) {
            delete m_shell;
        }
        
        if (m_process_manager) {
            delete m_process_manager;
        }
        
        if (m_config_parser) {
            delete m_config_parser;
        }
        
        CLogger::Close();
    }
    
    void Run() {
        m_running = true;
        
        // Iniciar programas con autostart
        CLogger::Info("Starting autostart programs...");
        m_process_manager->StartAutoStartPrograms(m_programs);
        
        // Iniciar shell
        m_shell->Start();
        
        // Loop principal
        MainLoop();
    }

private:
    void MainLoop() {
        CLogger::Info("Entering main loop");

        while (m_running) {
            // 1. Manejar cambios de estado de hijos (SIGCHLD)
            if (CSignalHandler::IsChildStatusChanged()) {
                m_process_manager->HandleChildStatusChange(m_programs);
                CSignalHandler::ResetChildStatus();
            }
            
            // 2. Monitorear procesos
            m_process_manager->MonitorProcesses(m_programs);
            
            // 3. Manejar SIGINT (Ctrl+C)
            if (CSignalHandler::IsSigintReceived()) {
                CSignalHandler::ResetSigint();
                std::cout << "\nReceived SIGINT. Type 'exit' to quit or continue working." << std::endl;
            }
            
            // 4. Manejar SIGHUP (reload config)
            if (CSignalHandler::IsSighupReceived()) {
                CSignalHandler::ResetSighup();
                CLogger::Info("SIGHUP received - configuration reload requested");
                // TODO: Implementar recarga de configuración
            }
            
            // 5. Procesar entrada del usuario si está disponible
            if (CTaskmasterShell::UserInputReady()) {
                int result = m_shell->PromptLoop();
                
                if (result == -1) {
                    // Usuario solicitó salida
                    m_running = false;
                    break;
                }
            }
            
            // 6. Pequeña pausa para no saturar la CPU
            usleep(50000); // 50ms
        }
        
        CLogger::Info("Exiting main loop");
        Shutdown();
    }
    
    void Shutdown() {
        CLogger::Info("Shutting down all processes...");
        
        // Detener todos los procesos
        for (const auto& program : m_programs) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                const ProcessInfo* info = 
                    m_process_manager->GetProcessInfo(program.GetName(), i);
                
                if (info && info->pid > 0) {
                    int signal = SIGTERM;
                    
                    // Obtener señal de parada del config
                    if (!program.GetConfig().stopsignal.empty()) {
                        // Aquí podrías parsear la señal
                    }
                    
                    m_process_manager->StopProcess(info->pid, signal, 0);
                }
            }
        }
        
        // Esperar un poco para que los procesos terminen
        sleep(1);
        
        // Forzar terminación si es necesario
        for (const auto& program : m_programs) {
            int numprocs = program.GetConfig().numprocs;
            
            for (int i = 0; i < numprocs; ++i) {
                const ProcessInfo* info = 
                    m_process_manager->GetProcessInfo(program.GetName(), i);
                
                if (info && info->pid > 0) {
                    kill(info->pid, SIGKILL);
                }
            }
        }
        
        CLogger::Info("All processes stopped");
    }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "❌ Usage: " << argv[0] << " <config_file.yaml>" << std::endl;
        return 1;
    }
    
    try {
        CTaskmaster taskmaster(argv[1]);
        taskmaster.Run();
        
        std::cout << "✅ TaskMaster shut down cleanly" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}