#include "parse_utils.hpp"
#include <sstream>

std::string trim(std::string s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

std::vector<int> parseIntList(std::string s) {
    std::vector<int> result;

    if (!s.empty() && s.front() == '[' && s.back() == ']')
        s = s.substr(1, s.size() - 2);

    std::stringstream ss(s);
    std::string tok;

    while (std::getline(ss, tok, ',')) {
        tok = trim(tok);
        if (!tok.empty())
            result.push_back(std::stoi(tok));
    }
    return result;
}

std::string removeInlineComment(std::string s) {
    bool inQuotes = false;

    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];

        if (c == '"') {
            inQuotes = !inQuotes;
        }

        if (c == '#' && !inQuotes) {
            return trim(s.substr(0, i));
        }
    }
    return trim(s);
}