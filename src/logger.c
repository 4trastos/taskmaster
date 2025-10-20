#include "taskmaster.h"
#include "ft_printf.h"

void    monitor_processes(t_program_config *config)
{
    t_program_config *current;

    current = config;
    while (current->next != NULL)
    {
        pthread_mutex_lock(&output_mutex);
        ft_printf("Estoy monitoreando config:\n");
        pthread_mutex_unlock(&output_mutex);
        
        // Lógica 1: Arranque
        if (current->process && current->process->pstate == STARTING && current->process->pid == 0)
        {
            // El estado deseado es STARTING, pero el PID es 0 (muerto o nunca arrancó)
            launch_process(config);
        }
        // Lógica 2: Detención
        if (current->process && current->process->pstate == STOPPING && current->process->pid > 0)
        {
            // El estado deseado es STOPPING, y el PID está vivo
            stop_process(config->process->pid, config->stopsignal);
            // NOTA: stop_process debe ser asíncrono y usar config->stoptime para SIGKILL
        }
        // Lógica 3: Comprobación de tiempo de vida (config->starttime)
        current = current->next;
    }
    return;
}