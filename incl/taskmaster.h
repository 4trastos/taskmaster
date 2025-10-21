#ifndef TASKMASTER_H
# define    TASKMASTER_H

# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <signal.h>
# include <limits.h>
# include <stdbool.h>
# include <termios.h>
# include <pthread.h>
# include <fcntl.h>
# include <poll.h>
# include <sys/errno.h>
# include <sys/types.h>
# include <sys/fcntl.h>
# include <sys/wait.h>
# include <sys/user.h>
# include <sys/wait.h>
# include <sys/ioctl.h>
# include <sys/select.h>
# include <readline/readline.h>
# include <readline/history.h>

# define PROMPT "TaskMaster> "
# define INCREASE 20

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
    STOPPED_PERM,       // Parada permanente
    UNKNOWN
}   t_process_state;

typedef struct s_process
{
    pid_t           pid;
    int             instance_id;
    time_t          start_time;
    int             restart_count;
    int             exit_code;
    int             stdout_fd;
    int             error_fd;
    t_process_state pstate;
}   t_process;

typedef struct s_program_config
{
    char                    *name;
    char                    *command;
    char                    **env;
    char                    *workingdir;
    char                    *stdout_path;
    char                    *stderr_path;
    int                     *exitcodes;
    int                     numbexicod;
    int                     numprocs;
    int                     starttime;
    int                     stoptime;
    int                     stopsignal;
    int                     startretries;
    bool                    autostart;
    mode_t                  umask;
    t_autorestart           autorestart;
    t_process               *process;
    struct s_program_config *next;
}   t_program_config;

extern char **environ;
extern pthread_mutex_t output_mutex;
extern volatile sig_atomic_t g_sigint_received;
extern volatile sig_atomic_t g_child_status_changed;

//*** Process && Taskmaster ***/

int     user_input_ready(void);
void    init_process_test(t_program_config *config, char **envp, int numb);
void    init_process_info(t_program_config *config);
void    taskmaster_main_loop(t_program_config *config);
void    start_autostart_programs(t_program_config *config);
void    monitor_processes(t_program_config *config);
void    launch_process(t_program_config *config);
void    abort_restart(t_program_config *config);
void    stop_process(pid_t pid, int stopsignal, int status);
void    restart_policy(t_program_config *config, int state);
void    stop_policy(pid_t pid, int stopsignal, int state);

//*** Parser logic ***/

int     get_number_of_program(char *filename);

//*** comunications & signals ***/

void    init_signal(void);
void    sigint_handler(int signum);
void    sigchld_handler(int signum);
void    set_stdin_nonblock(void);

//*** Shell ***/

char	*no_last_space(char *str);
int     prompt_loop(t_program_config *config);
int     status_comand(t_program_config *config, char *command);
void    child_status_change(t_program_config *config);

//*** auxiliary functions ***/

char*   substr(const char *src, int start, int length);
int     ft_strcmp(char *s1, char *s2);
size_t  ft_strlen(char *str);
void    ft_free(t_program_config *config);

//*** GNL ***/
char	*ft_strdup(char *s1);
char	*ft_strchr(char *s, int c);
int		get_next_line(int fd, char **line);
size_t	ft_strlen(char *str);
void	ft_bzero(void *s, size_t n);
void	*ft_calloc(size_t n, size_t b);
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