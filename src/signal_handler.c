#define _POSIX_C_SOURCE 200809L

#include "taskmaster.h"
#include "ft_printf.h"

void    init_signal(void)
{
    struct sigaction sa_int = {0};
    struct sigaction sa_chld = {0};

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;

    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    sigaction(SIGINT, &sa_int, NULL);
    sigaction(SIGCHLD, &sa_chld, NULL);
    signal(SIGQUIT, SIG_IGN);
}

void    sigchld_handler(int signum)
{
    (void)signum;
    g_child_status_changed = 1;
}

void    sigint_handler(int signum)
{
    (void)signum;
    g_sigint_received = 1;
}
