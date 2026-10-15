#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"

class Discount {
    std::string code_;
    double percent_ = 0.0;
    Money minAmount_;
    Date validUntil_;
    int usageLimit_ = 1;
    int used_ = 0;

public:
    Discount(std::string code, double percent, Money minAmount, Date validUntil, int usageLimit)
        : code_(std::move(code)), percent_(percent), minAmount_(std::move(minAmount)), validUntil_(validUntil),
          usageLimit_(usageLimit) {
        if (percent <= 0.0 || percent > 90.0) throw InvalidDataException("процент скидки от 0 до 90");
    }

    bool isValidOn(const Date& date) const { return !validUntil_.isBefore(date) && used_ < usageLimit_; }

    bool isApplicable(const Money& orderTotal, const Date& date) const {
        return isValidOn(date) && !minAmount_.isGreaterThan(orderTotal);
    }

    Money applyTo(const Money& orderTotal, const Date& date) const {
        if (!isValidOn(date)) throw DiscountExpiredException(code_);
        if (!isApplicable(orderTotal, date)) throw InvalidDataException("сумма заказа ниже минимальной для скидки");
        return orderTotal.subtract(orderTotal.percentOf(percent_));
    }

    void registerUse() { ++used_; }
    int remainingUses() const { return usageLimit_ - used_; }

    const std::string& getCode() const { return code_; }
    double getPercent() const { return percent_; }
    const Money& getMinAmount() const { return minAmount_; }
    const Date& getValidUntil() const { return validUntil_; }
};
