#ifndef TASKMASTER_H
# define    TASKMASTER_H

# include <limits.h>
# include <ctype.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <signal.h>
# include <sys/errno.h>
# include <sys/types.h>
# include <sys/fcntl.h>
# include <sys/wait.h>
# include <stdbool.h>
# include <sys/user.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 200
# endif

typedef enum s_autorestart
{
    AR_ALWAYS,
    AR_NEVER,
    AR_UNEXPECTED
}   t_autorestart;

typedef enum s_process_state
{
    STOPPED,            // Proceso detenido
    STARTING,           // Iniciando
    RUNNING,            // Ejecutándose correctamente
    BACKOFF,            // Reintentando arranque
    STOPPING,           // En proceso de parada
    EXITED,             // Terminado (esperando decisión)
    FATAL,              // Error fatal (sin más reintentos)
    UNKNOWN
}   t_process_state;

typedef struct s_program_config t_program_config;

typedef struct s_program_config
{
    char                *name;
    char                *command;
    char                **env;
    char                *workingdir;
    char                *stdout_path;
    char                *stderr_path;
    int                 *exitcodes;
    int                 numbexicod;
    int                 numprocs;
    int                 starttime;
    int                 stoptime;
    int                 stopsignal;
    int                 startretries;
    bool                autostart;
    //mode_t              umask;
    char                *umask;
    t_autorestart       autorestart;
    t_program_config    *next;
}   t_program_config;

typedef struct s_process
{
    pid_t           pid;
    int             instance_id;
    t_process_state pstate;
    time_t          start_time;
    int             restart_count;
    int             exit_code;
    int             stdout_fd;
    int             error_fd;
}   t_process;

//*** Binay logic ***/


//*** Parse logic ***/
int get_number_of_program(char *filename);
t_program_config init_program_config_structs(char * filename, int progam_index);

//*** comunications & signals ***/



//*** auxiliary functions ***/

size_t  ft_strlen(char *str);
char*   ft_substr(char *src, int start, int length);
int	    ft_atoi(const char *str);
int     ft_strcmp(const char *s1, const char *s2);
bool    ft_atob(const char *str);

//*** GNL ***/
int		get_next_line(int fd, char **line);
char	*ft_strchr(char *s, int c);
void	ft_bzero(void *s, size_t n);
void	*ft_calloc(size_t n, size_t b);
size_t	ft_strlen(char *str);
char	*ft_strdup(char *s1);
char	*ft_strjoin(char *board, char *buffer);

#endif


/* 
QUÉ HACE TASKMASTER REALMENTE:
Para cada programa en el YAML:

    Ejecuta el comando (fork() + exec())

    Vigila si se cae (con SIGCHLD y waitpid())

    Reinicia si es necesario (según autorestart)

    Redirige logs (según stdout/stderr)

    Responde a tus comandos (start/stop/restart) */