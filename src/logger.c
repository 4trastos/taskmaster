#include "taskmaster.h"
#include "ft_printf.h"

void    process_monitoring(t_program_config *config, char *command)
{
    (void)command;
    if (config->process && g_child_status_changed == 0)
    {
        pthread_mutex_lock(&output_mutex);
        ft_printf("Estoy dentro y monitoreo config:\n");
        ft_printf("[COMMAND] ( %s )\n", config->command);
        ft_printf("[NAME] ( %s )\n", config->name);
        pthread_mutex_unlock(&output_mutex);
    }
    return;
}

/* El sistema de logging de Taskmaster es el mecanismo que el propio daemon
utiliza para registrar sus decisiones internas, cambios de estado, y eventos de control. 
Es el diario de Taskmaster.

Es la "caja negra" que te permite saber por qué un proceso se reinició, por qué un comando falló, 
o cuándo se aplicó una recarga de configuración.


1. EL sistema de logging debe ser capaz de escribir eventos detallados con un timestamp y un nivel de severidad (INFO, WARNING, ERROR, FATAL).
Categoría	                Ejemplo de Mensaje a Loggear
Inicio/Parada del Daemon	[INFO] Taskmaster daemon starting up. [INFO] Configuration file 'config.yaml' loaded successfully. [FATAL] Cannot open configuration file. Exiting.
Recarga de Configuración	[INFO] Received SIGHUP signal. Initiating hot reload. [ERROR] Failed to validate new config file. Sticking to old configuration.
Ciclo de Vida del Proceso	[INFO] Program 'webserver' (instance 1) launched with PID 12345. [INFO] Program 'database' (instance 0) is entering RUNNING state.
Reinicio y Fallos	        [WARNING] Process 12345 (webserver) exited with code 1. Restarting (Policy: unexpected). [FATAL] Process 12345 (webserver) failed 3/3 retries. Entering FATAL state.
Comandos de Control	        [INFO] Shell command received: START database. [INFO] Shell command received: STOP webserver. Sending SIGTERM to PID 12345.
Errores de Sistema	        [ERROR] fork() failed for webserver: Resource temporarily unavailable.


2. Dos tipos de logs:
Tipo de Log	            Origen	                                        Contenido	                                                                                Destino
Log del Programa	    El proceso hijo gestionado (e.g., webserver).	printf, cout, errores de la aplicación.	                                                    Los archivos configurados en stdout y stderr (e.g., /var/log/webserver.out). (Ya cubierto en launch_process)
Log de Taskmaster	    El daemon Taskmaster Core.	                    Eventos del sistema, decisiones de reinicio, errores de fork(), comandos de shell.	        Un único archivo principal para el daemon (e.g., /var/log/taskmaster.log). (Lo que nos ocupa ahora) */