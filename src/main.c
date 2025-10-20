#include "taskmaster.h"
#include "ft_printf.h"

volatile sig_atomic_t g_sigint_received = 0;
volatile sig_atomic_t g_child_status_changed = 0;

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
