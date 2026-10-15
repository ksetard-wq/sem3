#pragma once
#include <string>
#include <utility>

class Address {
    std::string street_;
    std::string city_;
    std::string postalCode_;
    std::string country_;

public:
    Address() = default;
    Address(std::string street, std::string city, std::string postalCode, std::string country)
        : street_(std::move(street)), city_(std::move(city)),
          postalCode_(std::move(postalCode)), country_(std::move(country)) {}

    bool isComplete() const {
        return !street_.empty() && !city_.empty() && !postalCode_.empty() && !country_.empty();
    }
    std::string fullAddress() const { return country_ + ", " + postalCode_ + ", " + city_ + ", " + street_; }
    bool isSameCity(const Address& other) const { return city_ == other.city_ && country_ == other.country_; }
    bool isDomestic(const std::string& homeCountry) const { return country_ == homeCountry; }

    const std::string& getStreet() const { return street_; }
    const std::string& getCity() const { return city_; }
    const std::string& getPostalCode() const { return postalCode_; }
    const std::string& getCountry() const { return country_; }
};
