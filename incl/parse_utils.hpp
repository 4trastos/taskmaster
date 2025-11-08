#ifndef PARSEUTILS_H
# define PARSEUTILS_H

#include <string>
#include <vector>

std::string trim(std::string s);
std::vector<int> parseIntList(std::string s);
std::string removeInlineComment(std::string s);

#endif