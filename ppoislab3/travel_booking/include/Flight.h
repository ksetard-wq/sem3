#pragma once
#include <string>
#include <vector>
#include "Aircraft.h"
#include "Airline.h"
#include "Airport.h"
#include "Date.h"
#include "Money.h"
#include "Seat.h"

class Flight {
    std::string number_;
    Airline airline_;
    Airport origin_;
    Airport destination_;
    Date departure_;
    int departureHour_ = 0;
    int durationMinutes_ = 0;
    Aircraft aircraft_;
    std::vector<Seat> seats_;
    Money basePrice_;

public:
    Flight(std::string number, Airline airline, Airport origin, Airport destination, Date departure, int departureHour,
           int durationMinutes, Aircraft aircraft, Money basePrice)
        : number_(std::move(number)), airline_(std::move(airline)), origin_(std::move(origin)),
          destination_(std::move(destination)), departure_(departure), departureHour_(departureHour),
          durationMinutes_(durationMinutes), aircraft_(std::move(aircraft)), basePrice_(std::move(basePrice)) {
        if (origin_.getCode() == destination_.getCode()) throw InvalidDataException("аэропорты вылета и прилёта совпадают");
        if (departureHour_ < 0 || departureHour_ > 23 || durationMinutes_ <= 0) throw InvalidDataException("время рейса");
        buildSeats();
    }

    void buildSeats() {
        const char* businessLetters = "ACDF";
        const char* economyLetters = "ABCDEF";
        int businessRows = (aircraft_.getBusinessSeats() + 3) / 4;
        for (int i = 0; i < aircraft_.getBusinessSeats(); ++i) {
            seats_.emplace_back(i / 4 + 1, businessLetters[i % 4], CabinClass::Business);
        }
        for (int i = 0; i < aircraft_.getEconomySeats(); ++i) {
            seats_.emplace_back(businessRows + i / 6 + 1, economyLetters[i % 6], CabinClass::Economy);
        }
    }

    Seat& findSeat(const std::string& label) {
        for (auto& s : seats_) {
            if (s.label() == label) return s;
        }
        throw InvalidDataException("место " + label + " не найдено на рейсе " + number_);
    }

    int availableSeats(CabinClass cabin) const {
        int count = 0;
        for (const auto& s : seats_) {
            if (s.getCabin() == cabin && !s.getTaken()) ++count;
        }
        return count;
    }

    bool hasFreeSeat(CabinClass cabin) const { return availableSeats(cabin) > 0; }

    std::string reserveSeat(CabinClass cabin) {
        for (auto& s : seats_) {
            if (s.getCabin() == cabin && !s.getTaken()) {
                s.reserve();
                return s.label();
            }
        }
        throw NoSeatsAvailableException("рейс " + number_ + ", " + toString(cabin), 1, 0);
    }

    void reserveSpecificSeat(const std::string& label) { findSeat(label).reserve(); }
    void releaseSeat(const std::string& label) { findSeat(label).release(); }

    Money priceFor(CabinClass cabin) const { return basePrice_.percentOf(100.0 * priceMultiplier(cabin)); }

    Date arrivalDate() const { return departure_.addDays((departureHour_ * 60 + durationMinutes_) / 1440); }
    bool isInternational() const { return origin_.isInternationalTo(destination_); }
    double distanceKm() const { return origin_.distanceTo(destination_); }
    bool isFull() const { return !hasFreeSeat(CabinClass::Economy) && !hasFreeSeat(CabinClass::Business); }

    double occupancyPercent() const {
        int taken = 0;
        for (const auto& s : seats_) {
            if (s.getTaken()) ++taken;
        }
        return seats_.empty() ? 0.0 : 100.0 * taken / static_cast<double>(seats_.size());
    }

    bool departsOn(const Date& date) const { return departure_.toDayNumber() == date.toDayNumber(); }

    const std::string& getNumber() const { return number_; }
    const Airline& getAirline() const { return airline_; }
    const Airport& getOrigin() const { return origin_; }
    const Airport& getDestination() const { return destination_; }
    const Date& getDeparture() const { return departure_; }
    int getDepartureHour() const { return departureHour_; }
    int getDurationMinutes() const { return durationMinutes_; }
    const Aircraft& getAircraft() const { return aircraft_; }
    const Money& getBasePrice() const { return basePrice_; }
};
