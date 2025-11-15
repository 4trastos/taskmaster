#ifndef SIGNALHANDLER_HPP
#define SIGNALHANDLER_HPP

#include <csignal>
#include <atomic>

class CSignalHandler {
private:
    static std::atomic<bool> s_sigint_received;
    static std::atomic<bool> s_child_status_changed;
    static std::atomic<bool> s_sighup_received;

public:
    // Inicialización
    static void Init();
    
    // Handlers de señales
    static void SigintHandler(int signum);
    static void SigchldHandler(int signum);
    static void SighupHandler(int signum);
    
    // Getters para flags
    static bool IsSigintReceived();
    static bool IsChildStatusChanged();
    static bool IsSighupReceived();
    
    // Resetear flags
    static void ResetSigint();
    static void ResetChildStatus();
    static void ResetSighup();
    
    // Configuración de stdin no bloqueante
    static void SetStdinNonBlock();
    static void RestoreStdinBlock();

private:
    static struct termios s_old_term;
};

#endif // SIGNALHANDLER_HPP