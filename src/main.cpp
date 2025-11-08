#include "ConfigParser.hpp"

int main(int argc, char **argv, char **envp)
{
	(void)envp;
    if (argc != 2)
    {
        std::cout << "❌ Use: " << argv[0] <<  " must have PROG [ARGS] ❌" << std::endl;
        return (1);

    }

    CConfigParser* pConfigParser = new CConfigParser(argv[1]);

    for (const auto& program : pConfigParser->GetPrograms()){
        std::cout << program.GetName() << std::endl;
    }
    // 1. Parser de configuración YAML
	
    // 2. Estructuras de datos básicas
    // 3. Lanzamiento simple de procesos
    // 4. Shell interactiva básica

    // 5. Manejo de señales (SIGCHLD)
    // 6. Detección de estado de procesos
    // 7. Políticas de reinicio básicas
    // 8. Sistema de logging    (es la memoria del daemon)

    // 9. Múltiples instancias (numprocs)
    // 10. Recarga con SIGHUP
    // 11. Timeouts y reintentos
    // 12. Redirección de E/S robusta

    // 13. Manejo de edge cases
    // 14. Pruebas exhaustivas

    return (0);
}