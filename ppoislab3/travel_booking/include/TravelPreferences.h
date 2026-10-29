#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "CabinClass.h"
#include "Country.h"
#include "Money.h"

class TravelPreferences {
    Money budgetLimit_;
    CabinClass cabin_ = CabinClass::Economy;
    int maxStops_ = 1;
    std::vector<std::string> favoriteCountries_;
    bool directFlightsOnly_ = false;

public:
    TravelPreferences() = default;
    TravelPreferences(Money budgetLimit, CabinClass cabin, int maxStops)
        : budgetLimit_(std::move(budgetLimit)), cabin_(cabin), maxStops_(maxStops), directFlightsOnly_(maxStops == 0) {}

    bool fitsBudget(const Money& price) const { return !price.isGreaterThan(budgetLimit_); }
    bool acceptsStops(int stops) const { return stops <= maxStops_; }
    bool wantsComfort() const { return cabin_ == CabinClass::Business; }

    void addFavorite(const Country& country) {
        if (!isFavorite(country)) favoriteCountries_.push_back(country.getCode());
    }

    bool isFavorite(const Country& country) const {
        return std::find(favoriteCountries_.begin(), favoriteCountries_.end(), country.getCode()) != favoriteCountries_.end();
    }

    const Money& getBudgetLimit() const { return budgetLimit_; }
    CabinClass getCabin() const { return cabin_; }
    int getMaxStops() const { return maxStops_; }
    bool getDirectFlightsOnly() const { return directFlightsOnly_; }
};
