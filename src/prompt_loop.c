#include "taskmaster.h"
#include "ft_printf.h"

char	*no_last_space(char *str)
{
    size_t  len;

    if (!str)
        return (NULL);
    len = ft_strlen(str);
    if (len < 2)
        return (str);
    len --;
    while (str[len] == ' ' && len > 0)
    {
        str[len] = '\0';
        len --;
    }
    return (str);
}

int    status_comand(t_program_config *config, char *command)
{
    if (ft_strcmp("start", command) == 0)
        config->process->pstate = STARTING;
    else if (ft_strcmp("stop", command) == 0)
        config->process->pstate = STOPPING;
    else if (ft_strcmp("restart", command) == 0)
        config->process->pstate = STARTING;
    else
    {
        pthread_mutex_lock(&output_mutex);
        ft_printf("❌ Comando no reconocido. Use: [start] , [stop] o [restart] ❌\n");
        pthread_mutex_unlock(&output_mutex);
        return (-1);
    }
    return (0);

}

int    prompt_loop(t_program_config *config)
{
    char    *command;
    int     status;
    int     value = 0;
    
    command = no_last_space(readline(PROMPT));
    if (!command)
        return (-1);    // Ctrl+D (EOF) debe salir del programa
    add_history(command);
    if (ft_strcmp("exit", command) == 0)
    {
        config->process->pstate = STOPPING;
        value = -1;
    }
    else
    {
        status = status_comand(config, command);
        if (status == 0)
            value = 0;
    }
    free(command);
    return (value);
}
