#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// 3. test_sleep.c (Se ejecuta para siempre)

void    handle_signal(int sig)
{
    printf("🔔 Señal %d recibida - Terminando\n", sig);
    fflush(stdout); 
    exit (0);
}

int main(void)
{
    signal(SIGUSR1, handle_signal);
    signal(SIGTERM, handle_signal);

    printf("💤 PROGRAMA DURMIENDO - PID: %d\n", getpid());
    fflush(stdout);

    while (1)
    {
        sleep(5);
        printf("💤 Todavía vivo...\n");
        fflush(stdout);
    }
    
    return (0);
}