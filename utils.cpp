module;
#include "Windows.h"
module utils;

std::vector<std::string> split(const std::string& line, std::string delimeter)
{
    std::vector<std::string> result;

    size_t start = 0;
    size_t index = line.find(delimeter);
    while (index != std::string::npos)
    {
        result.push_back(line.substr(start, index - start));
        start = index + delimeter.size();
        index = line.find(delimeter, start);
    }
    if (start <= line.size())
    {
        result.push_back(line.substr(start));
    }

    return result;
}

std::u16string readUtf16File(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("can't open file");

    // Read BOM
    unsigned char bom[2];
    f.read(reinterpret_cast<char*>(bom), 2);
    if (!f) throw std::runtime_error("file too small");

    bool littleEndian = false;
    if (bom[0] == 0xFF && bom[1] == 0xFE) {
        littleEndian = true; // UTF-16 LE
    }
    else if (bom[0] == 0xFE && bom[1] == 0xFF) {
        littleEndian = false; // UTF-16 BE
    }
    else {
        throw std::runtime_error("no UTF-16 BOM");
    }

    // Read the rest
    std::u16string result;
    while (true) {
        char16_t ch;
        if (!f.read(reinterpret_cast<char*>(&ch), sizeof(ch))) break;

        if (!littleEndian) ch = (ch >> 8) | (ch << 8);

        result.push_back(ch);
    }

    return result;
}

std::string u16_to_utf8(const std::u16string& u16)
{
    if (u16.empty()) return {};

    int size = WideCharToMultiByte(
        CP_UTF8,
        0,
        reinterpret_cast<LPCWCH>(u16.data()),
        static_cast<int>(u16.size()),
        nullptr, 0,
        nullptr, nullptr
    );

    if (size <= 0) return {};

    std::string out(size, '\0');

    WideCharToMultiByte(
        CP_UTF8,
        0,
        reinterpret_cast<LPCWCH>(u16.data()),
        static_cast<int>(u16.size()),
        out.data(),
        size,
        nullptr, nullptr
    );

    return out;
}

std::string unquote(const std::string& str)
{
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"')
    {
        return str.substr(1, str.size() - 2);
    }
	return str;
}