#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Booking.h"
#include "Staff.h"

class TravelAgent : public Staff {
    double commissionPercent_ = 5.0;
    int salesCount_ = 0;
    std::vector<std::string> languages_;

public:
    TravelAgent(const Staff& base, double commissionPercent, std::vector<std::string> languages)
        : Staff(base), commissionPercent_(commissionPercent), languages_(std::move(languages)) {}

    Money commissionFor(const Money& amount) const { return amount.percentOf(commissionPercent_); }

    Money registerSale(const Booking& booking) {
        if (booking.getStatus() != BookingStatus::Paid) throw InvalidStateException("зарегистрировать продажу", toString(booking.getStatus()));
        ++salesCount_;
        return commissionFor(booking.getPrice());
    }

    bool speaks(const std::string& language) const {
        return std::find(languages_.begin(), languages_.end(), language) != languages_.end();
    }

    void addLanguage(const std::string& language) {
        if (!speaks(language)) languages_.push_back(language);
    }

    Money monthlyBonus() const { return salesCount_ >= 10 ? Money::fromUnits(5000) : Money::fromUnits(salesCount_ * 300); }

    std::string role() const override { return "Турагент"; }

    double getCommissionPercent() const { return commissionPercent_; }
    int getSalesCount() const { return salesCount_; }
    const std::vector<std::string>& getLanguages() const { return languages_; }
};
