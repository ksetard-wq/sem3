#pragma once
#include "Booking.h"
#include "CabinClass.h"

class FlightBooking : public Booking {
    std::string flightNumber_;
    std::string seatLabel_;
    CabinClass cabin_ = CabinClass::Economy;
    Date departure_;
    std::string route_;
    std::string passengerName_;

public:
    FlightBooking(std::string id, std::string travelerId, Date createdAt, Money price, std::string flightNumber,
                  std::string seatLabel, CabinClass cabin, Date departure, std::string route, std::string passengerName)
        : Booking(std::move(id), std::move(travelerId), createdAt, std::move(price)), flightNumber_(std::move(flightNumber)),
          seatLabel_(std::move(seatLabel)), cabin_(cabin), departure_(departure), route_(std::move(route)),
          passengerName_(std::move(passengerName)) {}

    std::string type() const override { return "Авиабилет"; }

    std::string describe() const override {
        return "Рейс " + flightNumber_ + " (" + route_ + "), " + departure_.toString() + ", место " + seatLabel_ + ", " +
               toString(cabin_) + ", пассажир " + passengerName_;
    }

    DateRange period() const override { return DateRange(departure_, departure_); }

    Money cancellationFee(const Date& today) const override {
        int days = today.daysUntil(departure_);
        double percent = days >= 14 ? 10.0 : days >= 3 ? 40.0 : days >= 1 ? 70.0 : 100.0;
        return getPrice().percentOf(percent);
    }

    std::string destination() const override {
        size_t dash = route_.find('-');
        return dash == std::string::npos ? route_ : route_.substr(dash + 1);
    }

    bool isCheckInOpen(const Date& today) const {
        int days = today.daysUntil(departure_);
        return days >= 0 && days <= 1;
    }

    int baggageAllowanceKg() const { return cabin_ == CabinClass::Business ? 40 : 23; }

    void changeSeat(const std::string& newLabel) {
        if (!isActive()) throw InvalidStateException("сменить место", toString(getStatus()));
        seatLabel_ = newLabel;
    }

    const std::string& getFlightNumber() const { return flightNumber_; }
    const std::string& getSeatLabel() const { return seatLabel_; }
    CabinClass getCabin() const { return cabin_; }
    const Date& getDeparture() const { return departure_; }
    const std::string& getRoute() const { return route_; }
    const std::string& getPassengerName() const { return passengerName_; }
};
