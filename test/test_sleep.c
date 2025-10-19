#include "../incl/taskmaster.h"
#include "../incl/ft_printf.h"

// 3. test_sleep.c (Se ejecuta para siempre)

void    handle_signal(int sig)
{
    ft_printf("🔔 Señal %d recibida - Terminando\n", sig);
    fflush(stdout); 
    exit (0);
}

int main(void)
{
    signal(SIGUSR1, handle_signal);
    signal(SIGTERM, handle_signal);

    ft_printf("💤 PROGRAMA DURMIENDO - PID: %d\n", getpid());
    fflush(stdout);

    while (1)
    {
        sleep(5);
        ft_printf("💤 Todavía vivo...\n");
        fflush(stdout);
    }
    
    return (0);
}