#pragma once
#include "Booking.h"

class ExcursionBooking : public Booking {
    std::string excursionId_;
    std::string title_;
    int places_ = 1;
    Date date_;

public:
    ExcursionBooking(std::string id, std::string travelerId, Date createdAt, Money price, std::string excursionId,
                     std::string title, int places, Date date)
        : Booking(std::move(id), std::move(travelerId), createdAt, std::move(price)), excursionId_(std::move(excursionId)),
          title_(std::move(title)), places_(places), date_(date) {}

    std::string type() const override { return "Экскурсия"; }
    std::string describe() const override { return "Экскурсия \"" + title_ + "\", " + date_.toString() + ", мест: " + std::to_string(places_); }
    DateRange period() const override { return DateRange(date_, date_); }

    Money cancellationFee(const Date& today) const override {
        int days = today.daysUntil(date_);
        return days >= 2 ? Money() : getPrice().percentOf(days >= 1 ? 50.0 : 100.0);
    }

    const std::string& getExcursionId() const { return excursionId_; }
    const std::string& getTitle() const { return title_; }
    int getPlaces() const { return places_; }
    const Date& getDate() const { return date_; }
};
