#pragma once
#include <cmath>
#include <map>
#include <string>
#include "Money.h"

class CurrencyConverter {
    std::string baseCurrency_ = "RUB";
    std::map<std::string, double> rates_;

public:
    CurrencyConverter() { rates_["RUB"] = 1.0; }
    explicit CurrencyConverter(std::string baseCurrency) : baseCurrency_(std::move(baseCurrency)) { rates_[baseCurrency_] = 1.0; }

    // Курс: сколько единиц базовой валюты стоит 1 единица указанной валюты.
    void setRate(const std::string& currency, double rateToBase) {
        if (rateToBase <= 0.0) throw InvalidDataException("курс валюты должен быть положительным");
        rates_[currency] = rateToBase;
    }

    bool supports(const std::string& currency) const { return rates_.count(currency) > 0; }

    Money convert(const Money& amount, const std::string& target) const {
        if (!supports(amount.getCurrency()) || !supports(target)) {
            throw InvalidDataException("нет курса для " + amount.getCurrency() + " -> " + target);
        }
        double inBase = static_cast<double>(amount.getCents()) * rates_.at(amount.getCurrency());
        return Money(std::llround(inBase / rates_.at(target)), target);
    }

    double rateOf(const std::string& currency) const {
        auto it = rates_.find(currency);
        if (it == rates_.end()) throw InvalidDataException("нет курса для " + currency);
        return it->second;
    }

    const std::string& getBaseCurrency() const { return baseCurrency_; }
};
