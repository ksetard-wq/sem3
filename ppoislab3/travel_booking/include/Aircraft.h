#pragma once
#include <string>
#include <utility>
#include "CabinClass.h"

class Aircraft {
    std::string model_;
    int economySeats_ = 0;
    int businessSeats_ = 0;
    int maxLuggageKg_ = 0;

public:
    Aircraft() = default;
    Aircraft(std::string model, int economySeats, int businessSeats, int maxLuggageKg)
        : model_(std::move(model)), economySeats_(economySeats), businessSeats_(businessSeats),
          maxLuggageKg_(maxLuggageKg) {}

    int totalSeats() const { return economySeats_ + businessSeats_; }
    int seatsIn(CabinClass cabin) const { return cabin == CabinClass::Economy ? economySeats_ : businessSeats_; }
    bool isWideBody() const { return totalSeats() >= 250; }
    int luggageAllowanceFor(CabinClass cabin) const { return cabin == CabinClass::Business ? 40 : 23; }

    const std::string& getModel() const { return model_; }
    int getEconomySeats() const { return economySeats_; }
    int getBusinessSeats() const { return businessSeats_; }
    int getMaxLuggageKg() const { return maxLuggageKg_; }
};
