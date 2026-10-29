#pragma once
#include "Booking.h"

class TourBooking : public Booking {
    std::string tourId_;
    std::string tourTitle_;
    int travelers_ = 1;
    DateRange tripPeriod_;
    std::string destinationCity_;

public:
    TourBooking(std::string id, std::string travelerId, Date createdAt, Money price, std::string tourId,
                std::string tourTitle, int travelers, DateRange tripPeriod, std::string destinationCity)
        : Booking(std::move(id), std::move(travelerId), createdAt, std::move(price)), tourId_(std::move(tourId)),
          tourTitle_(std::move(tourTitle)), travelers_(travelers), tripPeriod_(tripPeriod),
          destinationCity_(std::move(destinationCity)) {
        if (travelers_ < 1) throw InvalidDataException("число путешественников");
    }

    std::string type() const override { return "Тур"; }

    std::string describe() const override {
        return "Тур \"" + tourTitle_ + "\", " + tripPeriod_.toString() + ", путешественников: " + std::to_string(travelers_);
    }

    DateRange period() const override { return tripPeriod_; }
    bool isExclusive() const override { return true; }
    std::string destination() const override { return destinationCity_; }

    Money cancellationFee(const Date& today) const override {
        int days = today.daysUntil(tripPeriod_.getStart());
        double percent = days >= 30 ? 5.0 : days >= 14 ? 25.0 : days >= 7 ? 50.0 : days >= 0 ? 80.0 : 100.0;
        return getPrice().percentOf(percent);
    }

    Money perPerson() const { return getPrice().divide(travelers_); }

    void removeTravelers(int count) {
        if (count <= 0 || count >= travelers_) throw InvalidDataException("нельзя убрать столько путешественников");
        Money each = perPerson();
        travelers_ -= count;
        setPrice(each.multiply(travelers_));
    }

    const std::string& getTourId() const { return tourId_; }
    const std::string& getTourTitle() const { return tourTitle_; }
    int getTravelers() const { return travelers_; }
    const DateRange& getTripPeriod() const { return tripPeriod_; }
    const std::string& getDestinationCity() const { return destinationCity_; }
};
