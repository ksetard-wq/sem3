#pragma once
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

class Country {
    std::string code_;
    std::string name_;
    std::string currency_;
    std::vector<std::string> visaFreeFor_;

public:
    Country() = default;
    Country(std::string code, std::string name, std::string currency)
        : code_(std::move(code)), name_(std::move(name)), currency_(std::move(currency)) {}

    bool isSameCountry(const Country& other) const { return code_ == other.code_; }

    bool isVisaFreeFor(const Country& citizenship) const {
        return isSameCountry(citizenship) ||
               std::find(visaFreeFor_.begin(), visaFreeFor_.end(), citizenship.code_) != visaFreeFor_.end();
    }

    void addVisaFreeCountry(const Country& citizenship) {
        if (!isVisaFreeFor(citizenship)) visaFreeFor_.push_back(citizenship.code_);
    }

    bool usesCurrency(const std::string& currency) const { return currency_ == currency; }

    const std::string& getCode() const { return code_; }
    const std::string& getName() const { return name_; }
    const std::string& getCurrency() const { return currency_; }
};
