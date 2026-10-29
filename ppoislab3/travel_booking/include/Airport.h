#pragma once
#include <string>
#include <utility>
#include "City.h"

class Airport {
    std::string code_;
    std::string name_;
    City city_;
    int terminals_ = 1;

public:
    Airport() = default;
    Airport(std::string code, std::string name, City city, int terminals)
        : code_(std::move(code)), name_(std::move(name)), city_(std::move(city)), terminals_(terminals) {
        if (!isValidCode(code_)) throw InvalidDataException("код аэропорта IATA: " + code_);
    }

    static bool isValidCode(const std::string& code) {
        if (code.size() != 3) return false;
        for (char c : code) {
            if (c < 'A' || c > 'Z') return false;
        }
        return true;
    }

    double distanceTo(const Airport& other) const { return city_.distanceTo(other.city_); }
    bool isSameCity(const Airport& other) const { return city_.getId() == other.city_.getId(); }
    bool isInternationalTo(const Airport& other) const { return !city_.isSameCountry(other.city_); }

    const std::string& getCode() const { return code_; }
    const std::string& getName() const { return name_; }
    const City& getCity() const { return city_; }
    int getTerminals() const { return terminals_; }
};
