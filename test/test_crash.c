#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 1. test_crash.c (Se crashea inmediatamente)

int main(void)
{
    printf("🚨 PROGRAMA QUE SE CRASHEA - PID: %d\n", getpid());
    fflush(stdout);
    abort();        // Se crashea con SIGABRT
    return (0);
}