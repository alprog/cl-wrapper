export module utils;

import std;

export std::vector<std::string> split(const std::string& line, std::string delimeter);
export std::u16string readUtf16File(const std::string& path);
export std::string u16_to_utf8(const std::u16string& u16);
export std::string unquote(const std::string& str);