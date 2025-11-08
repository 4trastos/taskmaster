#include "ConfigParser.hpp"
#include "parse_utils.hpp"
#include <iostream>
#include <algorithm>


CConfigParser::CConfigParser(std::string const& filename)
: m_file(filename)
{
    if (!m_file.is_open()) {
        std::cerr << "Error al abrir archivo: " << filename << std::endl;
        return;
    }
    LoadConfigFromYaml();
}

CConfigParser::~CConfigParser() {}

CProgram CConfigParser::GetProgramByIndex(int index) const {
    if (index < 0 || index >= (int)m_programs.size())
        return CProgram{};
    return m_programs[index];
}

// -------------------------------------------------------------------------
// PARSER PRINCIPAL
// -------------------------------------------------------------------------

void CConfigParser::LoadConfigFromYaml() {
    std::string line;
    bool insidePrograms = false;

    CProgram current;
    bool inEnv = false;

    while (std::getline(m_file, line)) {
        std::string raw = removeInlineComment(trim(line));

        if (raw.empty() || raw[0] == '#')
            continue;

        if (!insidePrograms) {
            if (raw == "programs:") {
                insidePrograms = true;
            }
            continue;
        }

        int indent = line.find_first_not_of(' ');

        if (indent == 2 && raw.back() == ':') {
            if (!current.GetName().empty()) {
                m_programs.push_back(current);
                current = CProgram{};
            }
            current.SetName(raw.substr(0, raw.size() - 1));
            inEnv = false;
            continue;
        }

        if (indent == 4 && raw == "env:") {
            inEnv = true;
            continue;
        }

        if (indent == 4 && raw.find(':') != std::string::npos && !inEnv) {
            size_t colon = raw.find(':');
            std::string key = trim(raw.substr(0, colon));
            std::string value = trim(raw.substr(colon + 1));

            if (!value.empty() && value.front() == '"' && value.back() == '"')
                value = value.substr(1, value.size() - 2);

            auto& cfg = current.GetConfig();

            if (key == "cmd") cfg.cmd = value;
            else if (key == "numprocs") cfg.numprocs = std::stoi(value);
            else if (key == "autostart") cfg.autostart = (value == "true");
            else if (key == "autorestart") cfg.autorestart = CProgram::ParseAutorestart(value);
            else if (key == "exitcodes") cfg.exitcodes = parseIntList(value);
            else if (key == "startretries") cfg.startretries = std::stoi(value);
            else if (key == "starttime") cfg.starttime = std::stoi(value);
            else if (key == "stopsignal") cfg.stopsignal = value;
            else if (key == "stoptime") cfg.stoptime = std::stoi(value);
            else if (key == "stdout") cfg.stdout_path = value;
            else if (key == "stderr") cfg.stderr_path = value;
            else if (key == "workingdir") cfg.workingdir = value;
            else if (key == "umask") cfg.umask = value;

            continue;
        }

        if (indent == 6 && inEnv) {
            size_t colon = raw.find(':');
            if (colon == std::string::npos) continue;

            std::string key = trim(raw.substr(0, colon));
            std::string value = trim(raw.substr(colon + 1));

            if (!value.empty() && value.front() == '"' && value.back() == '"')
                value = value.substr(1, value.size() - 2);

            current.SetEnvField(key, value);
            continue;
        }
    }

    if (!current.GetName().empty()) {
        m_programs.push_back(current);
    }
}