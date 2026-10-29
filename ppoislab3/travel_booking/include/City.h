#pragma once
#include <cstdlib>
#include <string>
#include <utility>
#include "Country.h"
#include "GeoPoint.h"

class City {
    std::string id_;
    std::string name_;
    Country country_;
    GeoPoint location_;
    int utcOffset_ = 0;

public:
    City() = default;
    City(std::string id, std::string name, Country country, GeoPoint location, int utcOffset)
        : id_(std::move(id)), name_(std::move(name)), country_(std::move(country)), location_(location),
          utcOffset_(utcOffset) {}

    double distanceTo(const City& other) const { return location_.distanceKm(other.location_); }
    int timeDifferenceHours(const City& other) const { return std::abs(utcOffset_ - other.utcOffset_); }
    bool isInCountry(const Country& country) const { return country_.isSameCountry(country); }
    bool isSameCountry(const City& other) const { return country_.isSameCountry(other.country_); }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const Country& getCountry() const { return country_; }
    const GeoPoint& getLocation() const { return location_; }
    int getUtcOffset() const { return utcOffset_; }
};
