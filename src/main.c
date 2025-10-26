#include "taskmaster.h"
#include "ft_printf.h"

volatile sig_atomic_t g_sigint_received = 0;
volatile sig_atomic_t g_child_status_changed = 0;

void    start_autostart_programs(t_program_config *config)
{
    t_process   *process;
    char        *argv_exec[2];
    char        **envp_exec;

    process = malloc(sizeof(t_process));
    if (!process)
        return;

    argv_exec[0] = config->command;
    argv_exec[1] = NULL;
    envp_exec = (config->env) ? config->env : environ;

    process->pid = fork();
    if (process->pid == -1)
    {
        ft_printf("Error: Failed fork ( %s )\n", strerror(errno));
        free (process);
        return;
    }
    if (process->pid == 0)
    {
        if (execve(config->command, argv_exec, envp_exec) == -1)
        {
            ft_printf("Error: execve para %s ( %s )\n", config->name, strerror(errno));
            free(process);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        config->process = process;
        config->process->start_time = time(NULL);
        config->process->pstate = RUNNING;
        config->process->restart_count = 0;
        // pthread_mutex_lock(&output_mutex);
        // ft_printf("✅ Proceso '%s' iniciado con PID: %d\n", config->name, config->process->pid);
        // pthread_mutex_unlock(&output_mutex);
    }
}

int main(int argc, char **argv, char **envp)
{
    int                 numb_prog;
    t_program_config    *config;

    if (argc != 2)
    {
        ft_printf("❌ Use: %s must have PROG [ARGS] ❌\n", argv[0]);
        return (1);
    }
    
    numb_prog = get_number_of_program(argv[1]);
    config = malloc(sizeof(t_program_config));
    if (!config)
        return (1);
    
    init_signal();
    init_process_test(config, envp, numb_prog);
    start_autostart_programs(config);
    taskmaster_main_loop(config);
    ft_free (config);
    ft_printf("✅ Taskmaster se ha cerrado limpiamente.\n");
    return (0);
}



// // **Función de manejo de entrada (callback)**
// void command_input_handler(char *line)
// {
//     if (line == NULL)
//     {
//         // Esto se llama si el usuario presiona Ctrl+D (EOF)
//         // Puedes manejar la salida aquí o usar una bandera global.
//         g_sigint_received = 2; // Usamos 2 para indicar salida por EOF/exit
//         return;
//     }

//     if (*line)
//     {
//         // Aquí procesas el comando: start, stop, exit, etc.
//         // Reemplaza el contenido de tu antiguo prompt_loop (status_comand)
        
//         // Asumiendo que tienes acceso a 'config':
//         t_program_config *config = /* [Obtener la configuración global si es posible] */;
//         // ... Lógica de procesamiento de comando ...
        
//         // Ejemplo simplificado:
//         if (ft_strcmp("exit", line) == 0)
//             g_sigint_received = 2;
//         else
//             status_comand(config, line);
            
//         add_history(line);
//     }
    
//     free(line); // Readline asigna, tú liberas
// }

// void    taskmaster_main_loop(t_program_config *config)
// {
//     // *** Inicialización de Readline al inicio del bucle ***
//     // Esto configura readline en modo callback (asíncrono)
//     rl_callback_handler_install(PROMPT, command_input_handler);

//     while (1)
//     {
//         // Lógica de monitoreo y manejo de señales (PERMANECE IGUAL)
//         if (g_child_status_changed)
//             child_status_change(config);
            
//         monitor_processes(config);

//         if (g_sigint_received == 2) // Si se marcó 'exit' o Ctrl+D
//             break;
//         // ... (Manejo de SIGINT=1) ...
        
//         // *** 2. LECTURA ASÍNCRONA DE ENTRADA ***
//         // Reemplazamos 'user_input_ready' y 'prompt_loop' por esto:
//         if (user_input_ready()) // Sigue usando select para ver si hay datos en STDIN
//             rl_callback_read_char(); // Lee solo UN carácter, no bloquea.

//         // ...
//         else
//             usleep(50000); // Pequeña pausa para no quemar la CPU
//     }
    
//     // *** Desinstalación de Readline al final del bucle ***
//     rl_callback_handler_remove();
//     return;
// }