#pragma once
#include <cctype>
#include <string>
#include <utility>

class ContactInfo {
    std::string phone_;
    std::string email_;

public:
    ContactInfo() = default;
    ContactInfo(std::string phone, std::string email) : phone_(std::move(phone)), email_(std::move(email)) {}

    bool isEmailValid() const {
        size_t at = email_.find('@');
        if (at == std::string::npos || at == 0 || email_.find(' ') != std::string::npos) return false;
        size_t dot = email_.find('.', at);
        return dot != std::string::npos && dot > at + 1 && dot + 1 < email_.size();
    }

    bool isPhoneValid() const {
        int digits = 0;
        for (char c : phone_) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                ++digits;
            } else if (c != '+' && c != '-' && c != '(' && c != ')' && c != ' ') {
                return false;
            }
        }
        return digits >= 10 && digits <= 15;
    }

    bool hasAnyContact() const { return !phone_.empty() || !email_.empty(); }

    std::string maskedPhone() const {
        if (phone_.size() <= 4) return phone_;
        return std::string(phone_.size() - 4, '*') + phone_.substr(phone_.size() - 4);
    }

    const std::string& getPhone() const { return phone_; }
    const std::string& getEmail() const { return email_; }
};
