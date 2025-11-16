#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 2. test_normal.c (Termina normalmente)

int main(void)
{
    printf("✅ PROGRAMA NORMAL - Iniciando...\n");
    fflush(stdout);
    sleep(1);
    printf("✅ PROGRAMA NORMAL - Terminando con código 0\n");

    return (0);     // Terminación normal
}