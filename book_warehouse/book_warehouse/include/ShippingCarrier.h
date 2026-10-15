#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"
#include "Package.h"

class ShippingCarrier {
    std::string name_;
    Money baseFee_;
    Money ratePerKg_;
    int maxWeightGrams_ = 0;
    int avgDays_ = 0;

public:
    ShippingCarrier() = default;
    ShippingCarrier(std::string name, Money baseFee, Money ratePerKg, int maxWeightGrams, int avgDays)
        : name_(std::move(name)), baseFee_(std::move(baseFee)), ratePerKg_(std::move(ratePerKg)),
          maxWeightGrams_(maxWeightGrams), avgDays_(avgDays) {}

    bool canCarry(const Package& package) const { return package.getWeightGrams() <= maxWeightGrams_; }

    Money calculateCost(const Package& package) const {
        int kilograms = (package.getWeightGrams() + 999) / 1000;
        return baseFee_.add(ratePerKg_.multiply(kilograms));
    }

    Date estimateDelivery(const Date& shippedAt) const { return shippedAt.addDays(avgDays_); }
    bool isFasterThan(const ShippingCarrier& other) const { return avgDays_ < other.avgDays_; }

    const std::string& getName() const { return name_; }
    const Money& getBaseFee() const { return baseFee_; }
    const Money& getRatePerKg() const { return ratePerKg_; }
    int getMaxWeightGrams() const { return maxWeightGrams_; }
    int getAvgDays() const { return avgDays_; }
};
