#pragma once
#include <string>
#include <utility>
#include "DateRange.h"
#include "Money.h"

class PromoCode {
    std::string code_;
    double percent_ = 0.0;
    DateRange validity_;
    Money minAmount_;
    int usageLimit_ = 1;
    int used_ = 0;

public:
    PromoCode(std::string code, double percent, DateRange validity, Money minAmount, int usageLimit)
        : code_(std::move(code)), percent_(percent), validity_(validity), minAmount_(std::move(minAmount)),
          usageLimit_(usageLimit) {
        if (percent <= 0.0 || percent > 70.0) throw InvalidDataException("процент скидки от 0 до 70");
    }

    bool isValidOn(const Date& date) const { return validity_.contains(date) && used_ < usageLimit_; }
    bool isApplicable(const Money& amount, const Date& date) const { return isValidOn(date) && !minAmount_.isGreaterThan(amount); }

    Money applyTo(const Money& amount, const Date& date) const {
        if (!isApplicable(amount, date)) throw PromoCodeInvalidException(code_);
        return amount.subtract(amount.percentOf(percent_));
    }

    void registerUse() { ++used_; }
    int remainingUses() const { return usageLimit_ - used_; }

    const std::string& getCode() const { return code_; }
    double getPercent() const { return percent_; }
    const DateRange& getValidity() const { return validity_; }
    const Money& getMinAmount() const { return minAmount_; }
};
