#include "../incl/taskmaster.h"
#include "../incl/ft_printf.h"

// 2. test_normal.c (Termina normalmente)

int main(void)
{
    ft_printf("✅ PROGRAMA NORMAL - Iniciando...\n");
    fflush(stdout);
    sleep(1);
    ft_printf("✅ PROGRAMA NORMAL - Terminando con código 0\n");

    return (0);     // Terminación normal
}