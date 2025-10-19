#include "taskmaster.h"
#include "ft_printf.h"

void    init_signal(void)
{
    struct sigaction sa_int = {0};
    struct sigaction sa_chld = {0};

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;

    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    sigaction(SIGINT, &sa_int, NULL);
    sigaction(SIGCHLD, &sa_chld, NULL);
    signal(SIGQUIT, SIG_IGN);
}

void    child_status_change(t_program_config *config)
{
    pid_t   pid;
    int     status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (config->process && config->process->pid == pid)
        {
            pthread_mutex_lock(&output_mutex);
            ft_printf("\n⚠️ Proceso '%s' (PID %d) ha terminado.\n", config->name, config->process->pid);
            pthread_mutex_unlock(&output_mutex);
            config->process->pstate = STOPPED;
            if (config->autostart == true && (config->exitcodes[0] == 0 || config->exitcodes[1] == 1))
            {
                config->process->pstate = STARTING;
                config->process->restart_count++;
                break;
            }
        }
        if (WIFEXITED(status))
            ft_printf("✅ Estado de salida normal: %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            ft_printf("✅ Terminado por señal: %d\n", WTERMSIG(status));
        free(config->process);
        config->process = NULL;
    }

    g_child_status_changed = 0;
    rl_on_new_line();
    rl_redisplay();  
}

void    sigchld_handler(int signum)
{
    (void)signum;
    g_child_status_changed = 1;
}

void    sigint_handler(int signum)
{
    (void)signum;
    g_sigint_received = 1;
}
