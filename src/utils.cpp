#include "utils.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <limits>
#include <climits>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <csignal>
#include <cstring>

namespace Utils {

int StrCmp(const std::string& s1, const std::string& s2) {
    return s1.compare(s2);
}

int SafeAtoi(const std::string& str) {
    if (str.empty()) {
        return -1;
    }
    
    // Verificar que todos los caracteres sean dígitos
    for (char c : str) {
        if (!std::isdigit(c)) {
            return -1;
        }
    }
    
    try {
        long long num = std::stoll(str);
        
        if (num > std::numeric_limits<int>::max() || 
            num < std::numeric_limits<int>::min()) {
            return -1;
        }
        
        return static_cast<int>(num);
    } catch (...) {
        return -1;
    }
}

std::string SubStr(const std::string& src, size_t start, size_t length) {
    if (start >= src.length()) {
        return "";
    }
    
    if (start + length > src.length()) {
        length = src.length() - start;
    }
    
    return src.substr(start, length);
}

std::string TrimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    
    if (start == std::string::npos) {
        return "";
    }
    
    return str.substr(start);
}

std::string TrimRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    
    if (end == std::string::npos) {
        return "";
    }
    
    return str.substr(0, end + 1);
}

std::string Trim(const std::string& str) {
    return TrimLeft(TrimRight(str));
}

std::vector<std::string> Split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::vector<std::string> SplitWhitespace(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

int StringToSignal(const std::string& signal_name) {
    std::string upper_name = signal_name;
    std::transform(upper_name.begin(), upper_name.end(), 
                  upper_name.begin(), ::toupper);
    
    // Eliminar el prefijo "SIG" si existe
    if (upper_name.substr(0, 3) == "SIG") {
        upper_name = upper_name.substr(3);
    }
    
    if (upper_name == "TERM") return SIGTERM;
    if (upper_name == "KILL") return SIGKILL;
    if (upper_name == "INT") return SIGINT;
    if (upper_name == "HUP") return SIGHUP;
    if (upper_name == "QUIT") return SIGQUIT;
    if (upper_name == "USR1") return SIGUSR1;
    if (upper_name == "USR2") return SIGUSR2;
    if (upper_name == "STOP") return SIGSTOP;
    if (upper_name == "CONT") return SIGCONT;
    
    // Por defecto, SIGTERM
    return SIGTERM;
}

std::string SignalToString(int signal) {
    switch (signal) {
        case SIGTERM: return "SIGTERM";
        case SIGKILL: return "SIGKILL";
        case SIGINT: return "SIGINT";
        case SIGHUP: return "SIGHUP";
        case SIGQUIT: return "SIGQUIT";
        case SIGUSR1: return "SIGUSR1";
        case SIGUSR2: return "SIGUSR2";
        case SIGSTOP: return "SIGSTOP";
        case SIGCONT: return "SIGCONT";
        default: return "UNKNOWN";
    }
}

bool IsNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    
    return true;
}

bool FileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool DirectoryExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool IsValidPath(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    
    // Si es una ruta absoluta
    if (path[0] == '/') {
        return true;
    }
    
    // Si es una ruta relativa
    if (path.find("..") == std::string::npos && 
        path.find("//") == std::string::npos) {
        return true;
    }
    
    return false;
}

bool IsValidCommand(const std::string& command) {
    if (command.empty()) {
        return false;
    }
    
    // Verificar si el comando existe y es ejecutable
    if (access(command.c_str(), X_OK) == 0) {
        return true;
    }
    
    return false;
}

std::string GetAbsolutePath(const std::string& path) {
    char resolved_path[PATH_MAX];
    
    if (realpath(path.c_str(), resolved_path) != nullptr) {
        return std::string(resolved_path);
    }
    
    return path;
}

std::string FormatTime(time_t timestamp) {
    char buffer[80];
    struct tm* timeinfo = localtime(&timestamp);
    
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    return std::string(buffer);
}

time_t GetCurrentTime() {
    return time(nullptr);
}

} // namespace Utils