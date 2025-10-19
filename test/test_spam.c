#include "../incl/taskmaster.h"
#include "../incl/ft_printf.h"
// 4. test_spam_output.c (Produce mucho output)

int main(void)
{
    int count = 0;
    ft_printf("📢 INICIANDO SPAM OUTPUT\n");
    fflush(stdout);

    while (count < 100)
    {
        ft_printf("📢 Línea %d: Este es un mensaje de spam para probar redirección de output\n", count++);
        fflush(stdout);
        usleep(500000);
    }

    ft_printf("📢 FIN DEL SPAM\n");
    return (0);
}