#pragma once
#include "Booking.h"

class HotelBooking : public Booking {
    std::string hotelId_;
    std::string hotelName_;
    int roomNumber_ = 0;
    DateRange stay_;
    int guests_ = 1;
    std::string city_;

public:
    HotelBooking(std::string id, std::string travelerId, Date createdAt, Money price, std::string hotelId,
                 std::string hotelName, int roomNumber, DateRange stay, int guests, std::string city)
        : Booking(std::move(id), std::move(travelerId), createdAt, std::move(price)), hotelId_(std::move(hotelId)),
          hotelName_(std::move(hotelName)), roomNumber_(roomNumber), stay_(stay), guests_(guests), city_(std::move(city)) {
        if (stay_.nights() < 1) throw InvalidDateRangeException("для проживания нужна минимум одна ночь");
    }

    std::string type() const override { return "Отель"; }

    std::string describe() const override {
        return hotelName_ + " (" + city_ + "), номер " + std::to_string(roomNumber_) + ", " + stay_.toString() + ", " +
               std::to_string(stay_.nights()) + " ноч., гостей: " + std::to_string(guests_);
    }

    DateRange period() const override { return stay_; }
    bool isExclusive() const override { return true; }
    std::string destination() const override { return city_; }

    Money cancellationFee(const Date& today) const override {
        int days = today.daysUntil(stay_.getStart());
        if (days >= 3) return Money();
        if (days >= 0) return getPrice().divide(stay_.nights());
        return getPrice();
    }

    void extendStay(int nights, const Money& pricePerNight) {
        if (nights <= 0) throw InvalidDataException("продление должно быть на положительное число ночей");
        if (!isActive()) throw InvalidStateException("продлить проживание", toString(getStatus()));
        stay_ = stay_.extend(nights);
        setPrice(getPrice().add(pricePerNight.multiply(nights)));
    }

    int nightsCount() const { return stay_.nights(); }

    const std::string& getHotelId() const { return hotelId_; }
    const std::string& getHotelName() const { return hotelName_; }
    int getRoomNumber() const { return roomNumber_; }
    const DateRange& getStay() const { return stay_; }
    int getGuests() const { return guests_; }
    const std::string& getCity() const { return city_; }
};
