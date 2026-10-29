#pragma once
#include <string>
#include <utility>
#include "Country.h"
#include "Date.h"

class TravelAdvisory {
    Country country_;
    int level_ = 1;
    std::string message_;
    Date issuedAt_;

public:
    TravelAdvisory(Country country, int level, std::string message, Date issuedAt)
        : country_(std::move(country)), level_(level), message_(std::move(message)), issuedAt_(issuedAt) {
        if (level < 1 || level > 5) throw InvalidDataException("уровень предупреждения от 1 до 5");
    }

    bool isSafe() const { return level_ <= 2; }
    bool requiresWarning() const { return level_ >= 3; }
    bool forbidsTravel() const { return level_ >= 4; }
    bool isExpired(const Date& today, int days) const { return issuedAt_.daysUntil(today) > days; }

    void escalate() {
        if (level_ < 5) ++level_;
    }

    void relax() {
        if (level_ > 1) --level_;
    }

    const Country& getCountry() const { return country_; }
    int getLevel() const { return level_; }
    const std::string& getMessage() const { return message_; }
    const Date& getIssuedAt() const { return issuedAt_; }
};
