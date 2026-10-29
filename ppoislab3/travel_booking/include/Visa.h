#pragma once
#include <string>
#include <utility>
#include "Country.h"
#include "DateRange.h"

class Visa {
    Country country_;
    DateRange validity_;
    std::string type_;
    int entries_ = 1;

public:
    Visa() = default;
    Visa(Country country, DateRange validity, std::string type, int entries)
        : country_(std::move(country)), validity_(validity), type_(std::move(type)), entries_(entries) {}

    bool coversTrip(const DateRange& trip) const {
        return entries_ > 0 && !trip.getStart().isBefore(validity_.getStart()) && !validity_.getEnd().isBefore(trip.getEnd());
    }

    bool isActiveOn(const Date& date) const { return validity_.contains(date); }

    void useEntry() {
        if (entries_ <= 0) throw InvalidDocumentException("въездов по визе не осталось");
        --entries_;
    }

    int entriesLeft() const { return entries_; }

    const Country& getCountry() const { return country_; }
    const DateRange& getValidity() const { return validity_; }
    const std::string& getType() const { return type_; }
};
