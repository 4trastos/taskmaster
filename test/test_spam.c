#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 4. test_spam_output.c (Produce mucho output)

int main(void)
{
    int count = 0;
    printf("📢 INICIANDO SPAM OUTPUT\n");
    fflush(stdout);

    while (count < 100)
    {
        printf("📢 Línea %d: Este es un mensaje de spam para probar redirección de output\n", count++);
        fflush(stdout);
        usleep(500000);
    }

    printf("📢 FIN DEL SPAM\n");
    return (0);
}