#include "taskmaster.h"
#include "ft_printf.h"

pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER;

void    child_status_change(t_program_config *config)
{
    pid_t   pid;
    int     status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (config->process && config->process->pid == pid)
        {
            pthread_mutex_lock(&output_mutex);
            ft_printf("⚠️ Proceso '%s' (PID %d) ha terminado.\n", config->name, config->process->pid);
            pthread_mutex_unlock(&output_mutex);

            // --- 1. Proceso de Limpieza y Registro ---
            stop_process(config->process->pid, config->stopsignal, status);
            config->process->pid = 0;
            config->process->pstate = STOPPED;
            // --- 2. Aplicar Política de Reinicio ---
            restart_policy(config, status);
            // IMPORTANTE: Aquí se liberaría el proceso/limpiaría si NO va a haber reinicio.
            // Para simplificar, asumimos que el monitor/free manejará la limpieza si pstate es STOPPED.
            //break;
        }
        else
        {
            // if (WIFEXITED(status))
            //     ft_printf("✅ Estado de salida normal: %d\n", WEXITSTATUS(status));
            // else if (WIFSIGNALED(status))
            //     ft_printf("✅ Terminado por señal: %d\n", WTERMSIG(status));
        }
    }

    g_child_status_changed = 0;
    // Lógica corregida para manejo de SIGCHLD:
    rl_on_new_line();
    rl_redisplay();
}

int user_input_ready(void)
{
    struct timeval tv = {0, 50000}; // 50ms timeout
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    return (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0);
}

void    taskmaster_main_loop(t_program_config *config)
{
    while (1)
    {
        if (g_child_status_changed)
            child_status_change(config);
            
        monitor_processes(config);

        if (g_sigint_received)
        {
            g_sigint_received = 0;
            rl_replace_line("", 0);
            ft_printf("\n");
            rl_on_new_line();
            rl_redisplay();
        }
        if (user_input_ready())
        {
            if (prompt_loop(config) == -1)
                break;
        }
        else
            usleep(50000);
    }
    return;
}