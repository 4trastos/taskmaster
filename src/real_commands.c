#include "taskmaster.h"
#include "ft_printf.h"

bool    is_expected_exit(int exit_status, t_program_config *config)
{
    int i = 0;

    if (exit_status < 0)        // Si terminó por señal, no es un exit code normal.
        return (false);
    while (i < config->numbexicod)
    {
        if (config->exitcodes[i] == exit_status)
            return(true);
        i++;
    }
    return (false);
}

void    stop_policy(pid_t pid, int stopsignal, int state)
{
    pthread_mutex_lock(&output_mutex);

    if (WIFEXITED(state))
        ft_printf("✅ [LOG] Proceso PID %d terminó con código de salida: %d.\n", pid, WEXITSTATUS(state));
    else if (WIFSIGNALED(state))
        ft_printf("✅ [LOG] Proceso PID %d terminó por señal: %d (%s).\n", pid, WTERMSIG(state), strsignal(WTERMSIG(state)));
    else
        ft_printf("✅ [LOG] Proceso PID %d terminó en estado desconocido.\n", pid);

    if (stop_process(pid, stopsignal, state) < 0)
        ft_printf("✅ [LOG] Señal de parada configurada: %s\n", strsignal(stopsignal));
    pthread_mutex_unlock(&output_mutex);
    return;
}

void    restart_policy(t_program_config *config, int state)
{
    bool    expect;
    int     exit_status;

    if (config->autorestart == AR_NEVER)
        return;
    if (!WIFEXITED(state))
        expect = false;
    else
    {
        exit_status = WEXITSTATUS(state); 
        expect = is_expected_exit(exit_status, config);
    }

    if (config->autorestart == AR_ALWAYS)
        config->process->pstate = STARTING;
    else if (config->autorestart == AR_NEVER)
        config->process->pstate = STOPPED;
    else if (config->autorestart == AR_UNEXPECTED)
    {
        if (expect == false)
            config->process->pstate = STARTING; // Reiniciar SÓLO si NO fue esperado.
        else
            config->process->pstate = STOPPED;  // Salida esperada, no reiniciar
    }

    if (config->process->pstate == STARTING)
    {
        if (config->process->restart_count < config->startretries)
            config->process->restart_count++;
        else
        {
            config->process->pstate = STOPPED_PERM;
            //abort_restart(config);
        }
    }
    return;
}

void    abort_restart(t_program_config *config)
{
    (void)config;
    return;
}

void    launch_process(t_program_config *config)
{
    (void)config;
    return;
}

int    stop_process(pid_t pid, int stopsignal, int state)
{
    (void)pid;
    (void)stopsignal;
    (void)state;
    return(0);
}