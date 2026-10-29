#pragma once
#include <cctype>
#include <string>
#include <utility>
#include "Country.h"
#include "DateRange.h"

class Passport {
    std::string number_;
    std::string holderName_;
    Country issuingCountry_;
    Date expiry_;

public:
    Passport() = default;
    Passport(std::string number, std::string holderName, Country issuingCountry, Date expiry)
        : number_(std::move(number)), holderName_(std::move(holderName)), issuingCountry_(std::move(issuingCountry)),
          expiry_(expiry) {}

    bool isFormatCorrect() const {
        if (number_.size() != 9) return false;
        for (char c : number_) {
            if (!std::isalnum(static_cast<unsigned char>(c))) return false;
        }
        return true;
    }

    bool isValidOn(const Date& date) const { return !expiry_.isBefore(date); }

    // Многие страны требуют, чтобы паспорт действовал ещё 6 месяцев после поездки.
    bool isValidForTrip(const DateRange& trip) const { return !expiry_.isBefore(trip.getEnd().addDays(180)); }

    int daysToExpiry(const Date& today) const { return today.daysUntil(expiry_); }
    bool expiresSoon(const Date& today, int days) const { return daysToExpiry(today) <= days; }

    const std::string& getNumber() const { return number_; }
    const std::string& getHolderName() const { return holderName_; }
    const Country& getIssuingCountry() const { return issuingCountry_; }
    const Date& getExpiry() const { return expiry_; }
};
