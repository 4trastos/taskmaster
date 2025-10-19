#include "taskmaster.h"
#include "ft_printf.h"

pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER;

int user_input_ready(void)
{
    struct timeval  tv = {0, 0}; // Creamos un timeout de 0 segundos y 0 milesimas.
    fd_set          fds;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    // select devuelve el número de file descriptors listos. Si select devuelve > 0, al menos un FD está listo.
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
        return (FD_ISSET(STDIN_FILENO, &fds));
    return (0);
}

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
        config->process->pstate = STARTING;
        config->process->restart_count = 0;
        pthread_mutex_lock(&output_mutex);
        ft_printf("✅ Proceso '%s' iniciado con PID: %d\n", config->name, config->process->pid);
        pthread_mutex_unlock(&output_mutex);
    }
}


void    taskmaster_main_loop(t_program_config *config)
{
    while (1)
    {
        if (g_child_status_changed)
            child_status_change(config);

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
            if (!prompt_loop(config))
                break;
        }
        else
            usleep(10000);
    }
    return;
}