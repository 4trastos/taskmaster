# Guía de Migración C → C++

## 📊 Comparación Detallada

### 1. Gestión de Strings

#### ❌ Versión C (auxiliar.c)
```c
int ft_strcmp(char *s1, char *s2)
{
    int i;
    if (!s1 || !s2)
        return (-1);
    i = 0;
    while (s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i])
        i++;
    return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

size_t ft_strlen(char *str)
{
    size_t len = 0;
    while (str[len] != '\0')
        len++;
    return (len);
}
```

#### ✅ Versión C++ (Utils.cpp)
```cpp
namespace Utils {
    int StrCmp(const std::string& s1, const std::string& s2) {
        return s1.compare(s2);
    }
    
    // No necesitas strlen, std::string tiene .length()
}
```

**Ventajas:**
- No hay punteros nulos (`std::string` nunca es NULL)
- Gestión automática de memoria
- Funciones miembro (`s.length()`, `s.compare()`)

---

### 2. Get Next Line

#### ❌ Versión C (cool_get_next_line.c)
```c
int get_next_line(int fd, char **line)
{
    static char *board;
    int bytes_read;
    
    if (fd < 0 || BUFFER_SIZE <= 0 || !line)
        return (0);
    
    if (!board) {
        board = malloc(1);
        if (!board) {
            free(board);
            return (0);
        }
        board[0] = '\0';
    }
    // ... gestión manual de memoria
}
```

#### ✅ Versión C++ (recomendado)
```cpp
#include <fstream>
#include <string>

bool GetNextLine(std::ifstream& file, std::string& line) {
    return static_cast<bool>(std::getline(file, line));
}

// Uso:
std::ifstream file("config.yaml");
std::string line;
while (GetNextLine(file, line)) {
    // Procesar línea
}
```

**Ventajas:**
- No hay `malloc/free`
- RAII (el archivo se cierra automáticamente)
- Código mucho más simple

---

### 3. Estructuras de Datos

#### ❌ Versión C (taskmaster.h)
```c
typedef struct s_program_config
{
    char *name;
    char *command;
    char **env;
    int *exitcodes;
    int numbexicod;
    struct s_program_config *next;  // Lista enlazada manual
}   t_program_config;

// Uso:
t_program_config *config = malloc(sizeof(t_program_config));
config->name = strdup("nginx");
// ... más asignaciones manuales
```

#### ✅ Versión C++ (Program.hpp)
```cpp
class CProgram {
public:
    struct Config {
        std::string cmd;
        int numprocs = 1;
        bool autostart = false;
        std::vector<int> exitcodes;  // Vector dinámico
        // ... valores por defecto
    };

private:
    std::string m_name;
    Config m_config;
    std::map<std::string, std::string> m_env;  // Mapa clave-valor
};

// Uso:
CProgram program;
program.SetName("nginx");
program.GetConfig().cmd = "/usr/sbin/nginx";
program.SetEnvField("PATH", "/usr/bin");
```

**Ventajas:**
- No hay punteros nulos
- Valores por defecto en la estructura
- `std::vector` crece dinámicamente
- `std::map` para variables de entorno
- Destructor automático (no `ft_free`)

---

### 4. Gestión de Procesos

#### ❌ Versión C (process.c)
```c
void start_autostart_programs(t_program_config *config)
{
    t_process *process = malloc(sizeof(t_process));
    if (!process)
        return;
    
    char *argv_exec[2];
    argv_exec[0] = config->command;
    argv_exec[1] = NULL;
    
    process->pid = fork();
    if (process->pid == 0) {
        if (execve(config->command, argv_exec, envp_exec) == -1) {
            ft_printf("Error: execve para %s\n", config->name);
            free(process);
            exit(EXIT_FAILURE);
        }
    }
    config->process = process;
}
```

#### ✅ Versión C++ (ProcessManager.cpp)
```cpp
class CProcessManager {
private:
    std::map<std::string, std::vector<ProcessInfo>> m_processes;

public:
    bool LaunchProcess(const CProgram& program, int instance) {
        std::string key = GetProcessKey(program.GetName(), instance);
        ProcessInfo& info = m_processes[key][0];
        
        pid_t pid = fork();
        
        if (pid == 0) {
            // Proceso hijo
            SetupRedirections(program, info);
            
            std::vector<char*> argv_exec;
            argv_exec.push_back(const_cast<char*>(cfg.cmd.c_str()));
            argv_exec.push_back(nullptr);
            
            execve(cfg.cmd.c_str(), argv_exec.data(), envp_exec.data());
            
            std::cerr << "Error: execve failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        
        // Proceso padre
        info.pid = pid;
        info.start_time = time(nullptr);
        info.state = ProcessState::RUNNING;
        
        CLogger::Info("Process started with PID: " + std::to_string(pid));
        return true;
    }
};
```

**Ventajas:**
- Encapsulación en clase
- Gestión centralizada de procesos en `std::map`
- Logger integrado (`CLogger` en vez de `ft_printf`)
- Enum class para estados (`ProcessState::RUNNING`)
- Métodos claros y autodocumentados

---

### 5. Señales

#### ❌ Versión C (signal_handler.c)
```c
volatile sig_atomic_t g_sigint_received = 0;
volatile sig_atomic_t g_child_status_changed = 0;

void sigint_handler(int signum)
{
    (void)signum;
    g_sigint_received = 1;
}

void init_signal(void)
{
    struct sigaction sa_int = {0};
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);
}
```

#### ✅ Versión C++ (SignalHandler.cpp)
```cpp
class CSignalHandler {
private:
    static std::atomic<bool> s_sigint_received;
    static std::atomic<bool> s_child_status_changed;

public:
    static void Init() {
        struct sigaction sa_int = {};
        sa_int.sa_handler = SigintHandler;
        sigemptyset(&sa_int.sa_mask);
        
        if (sigaction(SIGINT, &sa_int, nullptr) == -1) {
            CLogger::Error("Failed to set SIGINT handler");
        }
    }
    
    static void SigintHandler(int signum) {
        (void)signum;
        s_sigint_received.store(true);
    }
    
    static bool IsSigintReceived() {
        return s_sigint_received.load();
    }
    
    static void ResetSigint() {
        s_sigint_received.store(false);
    }
};
```

**Ventajas:**
- `std::atomic` es thread-safe de forma estándar
- Encapsulación en clase estática
- Métodos getter/setter claros
- Manejo de errores con logger
- Mejor organización del código

---

### 6. Shell Interactiva

#### ❌ Versión C (prompt_loop.c)
```c
int prompt_loop(t_program_config *config)
{
    char *command = no_last_space(readline(PROMPT));
    if (!command)
        return (-1);
    
    add_history(command);
    
    if (ft_strcmp("exit", command) == 0) {
        config->process->pstate = STOPPING;
        value = -1;
    } else {
        status = status_comand(config, command);
    }
    
    free(command);
    return (value);
}

int status_comand(t_program_config *config, char *command)
{
    if (ft_strcmp("start", command) == 0)
        config->process->pstate = STARTING;
    else if (ft_strcmp("stop", command) == 0)
        config->process->pstate = STOPPING;
    // ...
}
```

#### ✅ Versión C++ (TaskmasterShell.cpp)
```cpp
class CTaskmasterShell {
private:
    CProcessManager* m_process_manager;
    std::vector<CProgram>* m_programs;

public:
    int PromptLoop() {
        char* line = readline(PROMPT);
        if (!line) return -1;
        
        std::string command = TrimString(line);
        
        if (!command.empty()) {
            add_history(line);
            
            if (!ProcessCommand(command)) {
                free(line);
                return -1;
            }
        }
        
        free(line);
        return 0;
    }
    
    bool ProcessCommand(const std::string& command) {
        std::vector<std::string> tokens = ParseCommand(command);
        
        std::string cmd = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        if (cmd == "start") return CmdStart(args);
        else if (cmd == "stop") return CmdStop(args);
        // ...
    }
    
    bool CmdStart(const std::vector<std::string>& args) {
        if (args.empty()) {
            std::cout << "Usage: start <program_name>" << std::endl;
            return true;
        }
        
        for (const auto& program : *m_programs) {
            if (program.GetName() == args[0]) {
                m_process_manager->SetProcessState(args[0], 0, 
                                                   ProcessState::STARTING);
                std::cout << "✅ Starting: " << args[0] << std::endl;
                return true;
            }
        }
        
        std::cout << "❌ Not found: " << args[0] << std::endl;
        return true;
    }
};
```

**Ventajas:**
- Separación de responsabilidades (cada comando es un método)
- `std::string` en vez de `char*`
- `std::vector` para parsear argumentos
- Código más legible y mantenible
- No hay `ft_strcmp` disperso

---

### 7. Main Loop

#### ❌ Versión C (shell.c)
```c
void taskmaster_main_loop(t_program_config *config)
{
    while (1) {
        if (g_child_status_changed)
            child_status_change(config);
        
        monitor_processes(config);
        
        if (g_sigint_received) {
            g_sigint_received = 0;
            rl_replace_line("", 0);
            ft_printf("\n");
            rl_on_new_line();
            rl_redisplay();
        }
        
        if (user_input_ready()) {
            if (prompt_loop(config) == -1)
                break;
        } else {
            usleep(50000);
        }
    }
}
```

#### ✅ Versión C++ (main_new.cpp)
```cpp
class CTaskmaster {
private:
    void MainLoop() {
        CLogger::Info("Entering main loop");
        
        while (m_running) {
            // 1. Manejar SIGCHLD
            if (CSignalHandler::IsChildStatusChanged()) {
                m_process_manager->HandleChildStatusChange(m_programs);
                CSignalHandler::ResetChildStatus();
                rl_on_new_line();
                rl_redisplay();
            }
            
            // 2. Monitorear procesos
            m_process_manager->MonitorProcesses(m_programs);
            
            // 3. Manejar SIGINT
            if (CSignalHandler::IsSigintReceived()) {
                CSignalHandler::ResetSigint();
                rl_replace_line("", 0);
                std::cout << std::endl;
                rl_on_new_line();
                rl_redisplay();
            }
            
            // 4. Procesar entrada
            if (CTaskmasterShell::UserInputReady()) {
                if (m_shell->PromptLoop() == -1) {
                    m_running = false;
                    break;
                }
            }
            
            usleep(50000);
        }
        
        Shutdown();
    }
};
```

**Ventajas:**
- Encapsulado en clase `CTaskmaster`
- Métodos con nombres descriptivos
- Logger integrado
- Separación clara de responsabilidades
- Método `Shutdown()` para limpieza

---

### 8. Logger

#### ❌ Versión C
```c
// Disperso por todo el código:
pthread_mutex_lock(&output_mutex);
ft_printf("✅ Proceso '%s' iniciado con PID: %d\n", name, pid);
pthread_mutex_unlock(&output_mutex);

// En otro lugar:
ft_printf("Error: Failed fork ( %s )\n", strerror(errno));
```

#### ✅ Versión C++ (Logger.cpp)
```cpp
class CLogger {
private:
    static std::mutex s_mutex;
    static std::ofstream s_log_file;
    
public:
    static void Init(const std::string& filename, LogLevel level);
    
    static void Info(const std::string& message) {
        Log(LogLevel::INFO, message);
    }
    
    static void Error(const std::string& message) {
        Log(LogLevel::ERROR, message);
    }
    
    static void Log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(s_mutex);
        
        std::string formatted = FormatMessage(level, message);
        
        if (s_log_to_console) {
            std::cout << formatted << std::endl;
        }
        
        if (s_log_to_file && s_log_file.is_open()) {
            s_log_file << formatted << std::endl;
            s_log_file.flush();
        }
    }
};

// Uso:
CLogger::Info("Process started with PID: " + std::to_string(pid));
CLogger::Error("Failed to fork: " + std::string(strerror(errno)));
```

**Ventajas:**
- Centralizado en una clase
- `std::lock_guard` para thread-safety automático
- Niveles de logging (DEBUG, INFO, WARNING, ERROR, FATAL)
- Timestamps automáticos
- Salida a consola y/o archivo
- Formato consistente

---

## 🎯 Resumen de Mejoras

| Aspecto | C | C++ |
|---------|---|-----|
| **Strings** | `char*`, funciones manuales | `std::string` |
| **Arrays** | Arrays fijos/malloc | `std::vector` |
| **Diccionarios** | Listas enlazadas | `std::map` |
| **Memoria** | `malloc/free` manual | RAII automático |
| **Encapsulación** | Structs + funciones | Clases con métodos |
| **Thread Safety** | `pthread_mutex_t` | `std::mutex`, `std::atomic` |
| **Logging** | `ft_printf` disperso | Clase `CLogger` |
| **Estados** | `#define` o `enum` | `enum class` |

## 🚀 Próximos Pasos

1. **Smart Pointers**: Cambiar `new/delete` por `std::unique_ptr` y `std::shared_ptr`
2. **Excepciones**: Manejar errores con try/catch
3. **STL Algorithms**: Usar `std::find_if`, `std::for_each`, etc.
4. **Lambda Functions**: Para callbacks y filtros
5. **Move Semantics**: Para optimizar rendimiento

## 📚 Recursos Recomendados

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)
- [cppreference.com](https://en.cppreference.com/)