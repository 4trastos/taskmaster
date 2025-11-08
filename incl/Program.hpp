#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <map>
#include <string>
#include <vector>

class CProgram {
public:

    enum class Autorestart {
        ALWAYS,
        NEVER,
        UNEXPECTED
    };

    struct Config {
        std::string cmd;
        int numprocs = 1;
        bool autostart = false;
        Autorestart autorestart = Autorestart::NEVER;
        std::vector<int> exitcodes;
        int startretries = 1;
        int starttime = 1;
        std::string stopsignal;
        int stoptime = 1;
        std::string stdout_path;
        std::string stderr_path;
        std::string workingdir;
        std::string umask;
    };

private:
    std::string m_name;
    Config m_config;
    std::map<std::string, std::string> m_env;

public:
    CProgram();
    ~CProgram();

    void SetName(const std::string& name) { m_name = name; }

    Config& GetConfig() { return m_config; }
    const Config& GetConfig() const { return m_config; }

    void SetEnvField(const std::string& key, const std::string& value) {
        m_env[key] = value;
    }

    const std::string& GetName() const { return m_name; }
    const std::map<std::string, std::string>& GetEnv() const { return m_env; }

    static Autorestart ParseAutorestart(const std::string& s){
        if (s == "always") return Autorestart::ALWAYS;
        if (s == "never") return Autorestart::NEVER;
        return Autorestart::UNEXPECTED;
    }
};

#endif