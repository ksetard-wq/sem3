#pragma once
#include <cctype>
#include <string>
#include "exceptions.h"

class ISBN {
    std::string code_;

public:
    explicit ISBN(const std::string& raw) : code_(normalize(raw)) {
        if (!isValidCode(code_)) throw InvalidISBNException(raw);
    }

    static std::string normalize(const std::string& raw) {
        std::string result;
        for (char c : raw) {
            if (std::isdigit(static_cast<unsigned char>(c))) result += c;
        }
        return result;
    }

    static int computeCheckDigit(const std::string& first12) {
        int sum = 0;
        for (size_t i = 0; i < 12; ++i) sum += (first12[i] - '0') * (i % 2 == 0 ? 1 : 3);
        return (10 - sum % 10) % 10;
    }

    static bool isValidCode(const std::string& digits) {
        return digits.size() == 13 && computeCheckDigit(digits.substr(0, 12)) == digits[12] - '0';
    }

    bool hasPrefix(const std::string& prefix) const { return code_.rfind(prefix, 0) == 0; }
    bool isRussianGroup() const { return hasPrefix("9785"); }
    bool equals(const ISBN& other) const { return code_ == other.code_; }
    std::string formatted() const { return code_.substr(0, 3) + "-" + code_.substr(3, 1) + "-" + code_.substr(4); }

    const std::string& getCode() const { return code_; }
};
