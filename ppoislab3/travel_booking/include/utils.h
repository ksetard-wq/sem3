#pragma once
#include <cctype>
#include <string>

// Приведение к нижнему регистру (ASCII + русские буквы в UTF-8).
inline std::string toLower(const std::string& s) {
    std::string r = s;
    for (size_t i = 0; i < r.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(r[i]);
        if (c < 0x80) {
            r[i] = static_cast<char>(std::tolower(c));
        } else if (c == 0xD0 && i + 1 < r.size()) {
            unsigned char n = static_cast<unsigned char>(r[i + 1]);
            if (n >= 0x90 && n <= 0x9F) {
                r[i + 1] = static_cast<char>(n + 0x20);
            } else if (n >= 0xA0 && n <= 0xAF) {
                r[i] = static_cast<char>(0xD1);
                r[i + 1] = static_cast<char>(n - 0x20);
            } else if (n == 0x81) {
                r[i] = static_cast<char>(0xD1);
                r[i + 1] = static_cast<char>(0x91);
            }
            ++i;
        } else if (c == 0xD1 && i + 1 < r.size()) {
            ++i;
        }
    }
    return r;
}

inline bool containsIgnoreCase(const std::string& text, const std::string& part) {
    return toLower(text).find(toLower(part)) != std::string::npos;
}
