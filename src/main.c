#include "taskmaster.h"
#include "ft_printf.h"

int main(int argc, char **argv, char **envp)
{
	(void)envp;
    if (argc != 2)
    {
        ft_printf("❌ Use: %s must have PROG [ARGS] ❌\n", argv[0]);
        return (1);

    }

    // 1. Parser de configuración YAML
	int n = get_number_of_program(argv[1]);
	t_program_config programs[n + 1];
	for (int i = 0; i < n; i++){
        ft_printf("Empieza a iniciar el programa %i, de %i programas\n", i, n);
		programs[i] = init_program_config_structs(argv[1], i);
		printf("Config: command %s\n", programs[i].command);
        printf("Config: numprocs %i\n", programs[i].numprocs);
        printf("Config: autostart %i\n", programs[i].autostart);
        printf("Config: exitcodes %i\n", programs[i].exitcodes[0]);
        printf("Config: startretries %i\n", programs[i].startretries);
        printf("Config: starttime %i\n", programs[i].starttime);
        printf("Config: stopsignal %i\n", programs[i].stopsignal);
        printf("Config: stoptiem %i\n", programs[i].stoptime);
        printf("Config: stdout %s\n", programs[i].stdout_path);
        printf("Config: stdeer %s\n", programs[i].stderr_path);
        printf("Config: workingdir %s\n", programs[i].workingdir);
        printf("Config: umask %s\n", programs[i].umask);
        printf("----------------------------------------------------\n");
	}
    // 2. Estructuras de datos básicas
    // 3. Lanzamiento simple de procesos
    // 4. Shell interactiva básica

    // 5. Manejo de señales (SIGCHLD)
    // 6. Detección de estado de procesos
    // 7. Políticas de reinicio básicas
    // 8. Sistema de logging    (es la memoria del daemon)

    // 9. Múltiples instancias (numprocs)
    // 10. Recarga con SIGHUP
    // 11. Timeouts y reintentos
    // 12. Redirección de E/S robusta

    // 13. Manejo de edge cases
    // 14. Pruebas exhaustivas

    return (0);
}