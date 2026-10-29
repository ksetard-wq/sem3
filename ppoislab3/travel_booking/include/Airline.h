#pragma once
#include <string>
#include <utility>
#include "Country.h"
#include "exceptions.h"

class Airline {
    std::string code_;
    std::string name_;
    Country country_;
    double rating_ = 3.0;

public:
    Airline() = default;
    Airline(std::string code, std::string name, Country country, double rating)
        : code_(std::move(code)), name_(std::move(name)), country_(std::move(country)), rating_(rating) {}

    bool isBasedIn(const Country& country) const { return country_.isSameCountry(country); }

    void rate(double rating) {
        if (rating < 0.0 || rating > 5.0) throw InvalidDataException("рейтинг авиакомпании от 0 до 5");
        rating_ = rating;
    }

    bool isReliable() const { return rating_ >= 4.0; }

    const std::string& getCode() const { return code_; }
    const std::string& getName() const { return name_; }
    const Country& getCountry() const { return country_; }
    double getRating() const { return rating_; }
};
