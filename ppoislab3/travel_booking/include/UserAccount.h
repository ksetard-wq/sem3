#pragma once
#include <cctype>
#include <functional>
#include <string>
#include <utility>
#include "Traveler.h"

class UserAccount {
    std::string login_;
    std::string passwordHash_;
    Traveler traveler_;
    bool active_ = true;
    int failedAttempts_ = 0;
    bool locked_ = false;

public:
    UserAccount(std::string login, const std::string& password, Traveler traveler)
        : login_(std::move(login)), traveler_(std::move(traveler)) {
        if (login_.size() < 3) throw InvalidDataException("логин короче 3 символов");
        if (!isStrongPassword(password)) throw WeakPasswordException();
        passwordHash_ = hashPassword(password);
    }

    static bool isStrongPassword(const std::string& password) {
        bool digit = false, upper = false, lower = false;
        for (char c : password) {
            unsigned char u = static_cast<unsigned char>(c);
            digit = digit || std::isdigit(u);
            upper = upper || std::isupper(u);
            lower = lower || std::islower(u);
        }
        return password.size() >= 8 && digit && upper && lower;
    }

    // Учебная имитация хеширования (в реальной системе нужен bcrypt/argon2).
    static std::string hashPassword(const std::string& password) {
        return std::to_string(std::hash<std::string>{}(password + "#travel-salt"));
    }

    void authenticate(const std::string& password) {
        if (!active_ || locked_) throw AccountLockedException(login_);
        if (hashPassword(password) == passwordHash_) {
            failedAttempts_ = 0;
            return;
        }
        if (++failedAttempts_ >= 3) {
            locked_ = true;
            throw AccountLockedException(login_);
        }
        throw AuthenticationFailedException(login_);
    }

    void changePassword(const std::string& oldPassword, const std::string& newPassword) {
        authenticate(oldPassword);
        if (!isStrongPassword(newPassword)) throw WeakPasswordException();
        passwordHash_ = hashPassword(newPassword);
    }

    void unlock() {
        locked_ = false;
        failedAttempts_ = 0;
    }

    void deactivate() { active_ = false; }
    bool isLocked() const { return locked_; }

    const std::string& getLogin() const { return login_; }
    Traveler& getTraveler() { return traveler_; }
    const Traveler& getTraveler() const { return traveler_; }
    bool getActive() const { return active_; }
    int getFailedAttempts() const { return failedAttempts_; }
};
