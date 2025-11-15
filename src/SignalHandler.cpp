#include "SignalHandler.hpp"
#include "Logger.hpp"
#include <unistd.h>
#include <termios.h>
#include <cstring>

// Inicialización de variables estáticas
std::atomic<bool> CSignalHandler::s_sigint_received(false);
std::atomic<bool> CSignalHandler::s_child_status_changed(false);
std::atomic<bool> CSignalHandler::s_sighup_received(false);
struct termios CSignalHandler::s_old_term;

void CSignalHandler::Init() {
    struct sigaction sa_int = {};
    struct sigaction sa_chld = {};
    struct sigaction sa_hup = {};
    
    // Configurar SIGINT (Ctrl+C)
    sa_int.sa_handler = SigintHandler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa_int, nullptr) == -1) {
        CLogger::Error("Failed to set SIGINT handler: " + 
                      std::string(strerror(errno)));
    }
    
    // Configurar SIGCHLD (cuando un hijo termina)
    sa_chld.sa_handler = SigchldHandler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    
    if (sigaction(SIGCHLD, &sa_chld, nullptr) == -1) {
        CLogger::Error("Failed to set SIGCHLD handler: " + 
                      std::string(strerror(errno)));
    }
    
    // Configurar SIGHUP (recarga de configuración)
    sa_hup.sa_handler = SighupHandler;
    sigemptyset(&sa_hup.sa_mask);
    sa_hup.sa_flags = SA_RESTART;
    
    if (sigaction(SIGHUP, &sa_hup, nullptr) == -1) {
        CLogger::Error("Failed to set SIGHUP handler: " + 
                      std::string(strerror(errno)));
    }
    
    // Ignorar SIGQUIT
    signal(SIGQUIT, SIG_IGN);
    
    CLogger::Debug("Signal handlers initialized");
}

void CSignalHandler::SigintHandler(int signum) {
    (void)signum;
    s_sigint_received.store(true);
}

void CSignalHandler::SigchldHandler(int signum) {
    (void)signum;
    s_child_status_changed.store(true);
}

void CSignalHandler::SighupHandler(int signum) {
    (void)signum;
    s_sighup_received.store(true);
}

bool CSignalHandler::IsSigintReceived() {
    return s_sigint_received.load();
}

bool CSignalHandler::IsChildStatusChanged() {
    return s_child_status_changed.load();
}

bool CSignalHandler::IsSighupReceived() {
    return s_sighup_received.load();
}

void CSignalHandler::ResetSigint() {
    s_sigint_received.store(false);
}

void CSignalHandler::ResetChildStatus() {
    s_child_status_changed.store(false);
}

void CSignalHandler::ResetSighup() {
    s_sighup_received.store(false);
}

void CSignalHandler::SetStdinNonBlock() {
    struct termios new_term;
    
    // Guardar configuración actual
    tcgetattr(STDIN_FILENO, &s_old_term);
    
    // Configurar modo no bloqueante
    new_term = s_old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    new_term.c_cc[VMIN] = 0;
    new_term.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
}

void CSignalHandler::RestoreStdinBlock() {
    tcsetattr(STDIN_FILENO, TCSANOW, &s_old_term);
}