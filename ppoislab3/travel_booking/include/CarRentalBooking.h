#pragma once
#include "Booking.h"

class CarRentalBooking : public Booking {
    std::string carId_;
    std::string carModel_;
    DateRange rental_;
    std::string pickupCity_;

public:
    CarRentalBooking(std::string id, std::string travelerId, Date createdAt, Money price, std::string carId,
                     std::string carModel, DateRange rental, std::string pickupCity)
        : Booking(std::move(id), std::move(travelerId), createdAt, std::move(price)), carId_(std::move(carId)),
          carModel_(std::move(carModel)), rental_(rental), pickupCity_(std::move(pickupCity)) {}

    std::string type() const override { return "Аренда авто"; }
    std::string describe() const override { return carModel_ + ", " + pickupCity_ + ", " + rental_.toString(); }
    DateRange period() const override { return rental_; }
    std::string destination() const override { return pickupCity_; }

    Money cancellationFee(const Date& today) const override {
        if (today.daysUntil(rental_.getStart()) >= 1) return Money();
        return getPrice().divide(rental_.nights() < 1 ? 1 : rental_.nights());
    }

    // Просрочка возврата: 150% суточной цены, пропорционально часам.
    Money lateReturnFee(int hoursLate, const Money& dailyPrice) const {
        if (hoursLate <= 0) return Money();
        return dailyPrice.percentOf(150.0 * hoursLate / 24.0);
    }

    const std::string& getCarId() const { return carId_; }
    const std::string& getCarModel() const { return carModel_; }
    const DateRange& getRental() const { return rental_; }
    const std::string& getPickupCity() const { return pickupCity_; }
};
