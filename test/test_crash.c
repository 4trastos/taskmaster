#include "../incl/taskmaster.h"
#include "../lib/printf/ft_printf.h"

// 1. test_crash.c (Se crashea inmediatamente)

int main(void)
{
    ft_printf("🚨 PROGRAMA QUE SE CRASHEA - PID: %d\n", getpid());
    fflush(stdout);
    abort();        // Se crashea con SIGABRT
    return (0);
}