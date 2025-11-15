#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

namespace Utils {
    // Comparación de strings
    int StrCmp(const std::string& s1, const std::string& s2);
    
    // Conversión de string a int segura
    int SafeAtoi(const std::string& str);
    
    // Extracción de substring
    std::string SubStr(const std::string& src, size_t start, size_t length);
    
    // Trim de espacios
    std::string Trim(const std::string& str);
    std::string TrimLeft(const std::string& str);
    std::string TrimRight(const std::string& str);
    
    // Split de strings
    std::vector<std::string> Split(const std::string& str, char delimiter);
    std::vector<std::string> SplitWhitespace(const std::string& str);
    
    // Conversión de señales
    int StringToSignal(const std::string& signal_name);
    std::string SignalToString(int signal);
    
    // Validaciones
    bool IsValidPath(const std::string& path);
    bool IsValidCommand(const std::string& command);
    bool IsNumeric(const std::string& str);
    
    // File utilities
    bool FileExists(const std::string& path);
    bool DirectoryExists(const std::string& path);
    std::string GetAbsolutePath(const std::string& path);
    
    // Time utilities
    std::string FormatTime(time_t timestamp);
    time_t GetCurrentTime();
}

#endif // UTILS_HPP