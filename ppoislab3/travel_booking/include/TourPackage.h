#pragma once
#include <string>
#include <vector>
#include "DateRange.h"
#include "Excursion.h"
#include "Flight.h"
#include "Hotel.h"

class TourPackage {
    std::string id_;
    std::string title_;
    City destination_;
    DateRange period_;
    Hotel hotel_;
    Flight outbound_;
    Flight inbound_;
    std::vector<Excursion> excursions_;
    Money basePrice_;
    int maxTravelers_ = 0;
    int booked_ = 0;

public:
    TourPackage(std::string id, std::string title, City destination, DateRange period, Hotel hotel, Flight outbound,
                Flight inbound, Money basePrice, int maxTravelers)
        : id_(std::move(id)), title_(std::move(title)), destination_(std::move(destination)), period_(period),
          hotel_(std::move(hotel)), outbound_(std::move(outbound)), inbound_(std::move(inbound)),
          basePrice_(std::move(basePrice)), maxTravelers_(maxTravelers) {
        validateItinerary();
    }

    void validateItinerary() const {
        if (outbound_.getDestination().getCity().getId() != destination_.getId()) {
            throw InvalidDataException("рейс туда не прилетает в город назначения тура");
        }
        if (inbound_.getOrigin().getCity().getId() != destination_.getId()) {
            throw InvalidDataException("обратный рейс вылетает не из города назначения тура");
        }
        if (hotel_.getCity().getId() != destination_.getId()) throw InvalidDataException("отель находится в другом городе");
        if (period_.getStart().isBefore(outbound_.getDeparture())) throw InvalidDateRangeException("рейс туда позже начала тура");
        if (inbound_.getDeparture().isBefore(period_.getEnd())) throw InvalidDateRangeException("обратный рейс раньше конца тура");
        if (maxTravelers_ < 1) throw InvalidDataException("вместимость тура");
    }

    void addExcursion(const Excursion& excursion) {
        if (!period_.contains(excursion.getDate())) {
            throw InvalidDateRangeException("экскурсия " + excursion.getDate().toString() + " вне периода тура");
        }
        excursions_.push_back(excursion);
    }

    int durationNights() const { return period_.nights(); }
    int freePlaces() const { return maxTravelers_ - booked_; }

    void reservePlaces(int travelers) {
        if (travelers <= 0) throw InvalidDataException("число путешественников должно быть положительным");
        if (travelers > freePlaces()) throw NoSeatsAvailableException("тур " + title_, travelers, freePlaces());
        booked_ += travelers;
    }

    void releasePlaces(int travelers) {
        if (travelers <= 0 || travelers > booked_) throw InvalidDataException("нельзя освободить столько мест");
        booked_ -= travelers;
    }

    Money pricePerPerson() const {
        Money sum = basePrice_;
        for (const auto& e : excursions_) sum = sum.add(e.getPrice());
        return sum;
    }

    Money totalPrice(int travelers) const { return pricePerPerson().multiply(travelers); }
    bool isInternational() const { return outbound_.isInternational(); }

    const std::string& getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const City& getDestination() const { return destination_; }
    const DateRange& getPeriod() const { return period_; }
    const Hotel& getHotel() const { return hotel_; }
    const Flight& getOutbound() const { return outbound_; }
    const Flight& getInbound() const { return inbound_; }
    const std::vector<Excursion>& getExcursions() const { return excursions_; }
    const Money& getBasePrice() const { return basePrice_; }
    int getMaxTravelers() const { return maxTravelers_; }
    int getBooked() const { return booked_; }
};
