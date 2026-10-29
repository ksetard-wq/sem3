#pragma once
#include <string>
#include <utility>
#include "City.h"
#include "DateRange.h"
#include "Money.h"

class CarRental {
    std::string id_;
    std::string model_;
    City pickupCity_;
    Money dailyPrice_;
    int seats_ = 4;
    bool available_ = true;

public:
    CarRental(std::string id, std::string model, City pickupCity, Money dailyPrice, int seats)
        : id_(std::move(id)), model_(std::move(model)), pickupCity_(std::move(pickupCity)),
          dailyPrice_(std::move(dailyPrice)), seats_(seats) {}

    Money costFor(const DateRange& period) const { return dailyPrice_.multiply(period.nights() < 1 ? 1 : period.nights()); }

    void reserve() {
        if (!available_) throw InvalidStateException("арендовать автомобиль " + model_, "уже занят");
        available_ = false;
    }

    void giveBack() { available_ = true; }
    bool isAvailableIn(const City& city) const { return available_ && pickupCity_.getId() == city.getId(); }
    bool fitsPassengers(int passengers) const { return passengers <= seats_; }

    const std::string& getId() const { return id_; }
    const std::string& getModel() const { return model_; }
    const City& getPickupCity() const { return pickupCity_; }
    const Money& getDailyPrice() const { return dailyPrice_; }
    int getSeats() const { return seats_; }
    bool getAvailable() const { return available_; }
};
