#pragma once
#include <cmath>
#include <cstdlib>
#include <string>
#include <utility>
#include "exceptions.h"

class Money {
    long long cents_ = 0;
    std::string currency_ = "RUB";

public:
    Money() = default;
    explicit Money(long long cents, std::string currency = "RUB") : cents_(cents), currency_(std::move(currency)) {}

    static Money fromUnits(double units, const std::string& currency = "RUB") {
        return Money(std::llround(units * 100.0), currency);
    }

    void requireSameCurrency(const Money& other) const {
        if (currency_ != other.currency_) {
            throw InvalidDataException("разные валюты: " + currency_ + " и " + other.currency_);
        }
    }

    Money add(const Money& other) const {
        requireSameCurrency(other);
        return Money(cents_ + other.cents_, currency_);
    }

    Money subtract(const Money& other) const {
        requireSameCurrency(other);
        return Money(cents_ - other.cents_, currency_);
    }

    Money multiply(int factor) const { return Money(cents_ * factor, currency_); }
    Money percentOf(double percent) const { return Money(std::llround(cents_ * percent / 100.0), currency_); }

    bool isGreaterThan(const Money& other) const {
        requireSameCurrency(other);
        return cents_ > other.cents_;
    }

    bool isZero() const { return cents_ == 0; }
    bool isNegative() const { return cents_ < 0; }

    std::string toString() const {
        long long absCents = std::llabs(cents_);
        long long rest = absCents % 100;
        return std::string(cents_ < 0 ? "-" : "") + std::to_string(absCents / 100) + "." +
               (rest < 10 ? "0" : "") + std::to_string(rest) + " " + currency_;
    }

    long long getCents() const { return cents_; }
    const std::string& getCurrency() const { return currency_; }
};
