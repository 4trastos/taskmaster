#include "Logger.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

// Inicialización de variables estáticas
std::mutex CLogger::s_mutex;
LogLevel CLogger::s_min_level = LogLevel::INFO;
std::ofstream CLogger::s_log_file;
bool CLogger::s_log_to_file = false;
bool CLogger::s_log_to_console = true;

void CLogger::Init(const std::string& filename, LogLevel min_level) {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    s_min_level = min_level;
    
    if (!filename.empty()) {
        s_log_file.open(filename, std::ios::app);
        if (s_log_file.is_open()) {
            s_log_to_file = true;
        }
    }
}

void CLogger::SetMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_min_level = level;
}

void CLogger::EnableFileLogging(const std::string& filename) {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (s_log_file.is_open()) {
        s_log_file.close();
    }
    
    s_log_file.open(filename, std::ios::app);
    if (s_log_file.is_open()) {
        s_log_to_file = true;
    }
}

void CLogger::EnableConsoleLogging(bool enable) {
    std::lock_guard<std::mutex> lock(s_mutex);
    s_log_to_console = enable;
}

void CLogger::Close() {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    if (s_log_file.is_open()) {
        s_log_file.close();
        s_log_to_file = false;
    }
}

std::string CLogger::GetTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* local_time = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
    
    return oss.str();
}

std::string CLogger::GetLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

std::string CLogger::FormatMessage(LogLevel level, const std::string& message) {
    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] "
        << "[" << GetLevelString(level) << "] "
        << message;
    
    return oss.str();
}

void CLogger::Log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(s_mutex);
    
    // Filtrar por nivel mínimo
    if (level < s_min_level) {
        return;
    }
    
    std::string formatted = FormatMessage(level, message);
    
    // Escribir en consola
    if (s_log_to_console) {
        if (level >= LogLevel::ERROR) {
            std::cerr << formatted << std::endl;
        } else {
            std::cout << formatted << std::endl;
        }
    }
    
    // Escribir en archivo
    if (s_log_to_file && s_log_file.is_open()) {
        s_log_file << formatted << std::endl;
        s_log_file.flush();
    }
}

void CLogger::Debug(const std::string& message) {
    Log(LogLevel::DEBUG, message);
}

void CLogger::Info(const std::string& message) {
    Log(LogLevel::INFO, message);
}

void CLogger::Warning(const std::string& message) {
    Log(LogLevel::WARNING, message);
}

void CLogger::Error(const std::string& message) {
    Log(LogLevel::ERROR, message);
}

void CLogger::Fatal(const std::string& message) {
    Log(LogLevel::FATAL, message);
}