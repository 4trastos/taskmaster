#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include <sstream>
# include <iostream>
# include <fstream>
# include <vector>

# include "Program.hpp"

class CConfigParser {
private:
    std::vector<CProgram> m_programs;
    std::ifstream m_file;

public:
    CConfigParser(std::string const& filename);
    ~CConfigParser();

    std::vector<CProgram> GetPrograms() const {return m_programs;}
    CProgram GetProgramByIndex(int index) const;

private:
    void LoadConfigFromYaml();
};

#endif //CONFIGPARSER_HPP