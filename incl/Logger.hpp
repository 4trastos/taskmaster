#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <iostream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

class CLogger {
private:
    static std::mutex s_mutex;
    static LogLevel s_min_level;
    static std::ofstream s_log_file;
    static bool s_log_to_file;
    static bool s_log_to_console;

public:
    // Configuración
    static void Init(const std::string& filename = "", 
                    LogLevel min_level = LogLevel::INFO);
    static void SetMinLevel(LogLevel level);
    static void EnableFileLogging(const std::string& filename);
    static void EnableConsoleLogging(bool enable = true);
    static void Close();
    
    // Métodos de logging
    static void Debug(const std::string& message);
    static void Info(const std::string& message);
    static void Warning(const std::string& message);
    static void Error(const std::string& message);
    static void Fatal(const std::string& message);
    
    static void Log(LogLevel level, const std::string& message);

private:
    static std::string GetLevelString(LogLevel level);
    static std::string GetTimestamp();
    static std::string FormatMessage(LogLevel level, const std::string& message);
};

#endif // LOGGER_HPP