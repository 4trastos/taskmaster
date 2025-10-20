#include "taskmaster.h"
#include "ft_printf.h"

void    ft_free(t_program_config *config)
{
    t_program_config    *current;
    t_program_config    *next_node;
    current = config;
    while (current)
    {
        next_node = current->next;
        if (current->process != NULL)
            free(current->process);
        free(current);
        current = next_node;
    }
}