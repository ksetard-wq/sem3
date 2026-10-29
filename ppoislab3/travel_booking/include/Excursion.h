#pragma once
#include <string>
#include <utility>
#include "Attraction.h"
#include "Date.h"
#include "Guide.h"

class Excursion {
    std::string id_;
    std::string title_;
    Attraction attraction_;
    Guide guide_;
    Date date_;
    int capacity_ = 0;
    int booked_ = 0;
    Money price_;

public:
    Excursion(std::string id, std::string title, Attraction attraction, Guide guide, Date date, int capacity, Money price)
        : id_(std::move(id)), title_(std::move(title)), attraction_(std::move(attraction)), guide_(std::move(guide)),
          date_(date), capacity_(capacity), price_(std::move(price)) {
        if (capacity < 1) throw InvalidDataException("вместимость экскурсии");
    }

    int freePlaces() const { return capacity_ - booked_; }
    bool isFull() const { return freePlaces() <= 0; }
    bool isOnDate(const Date& date) const { return date_.toDayNumber() == date.toDayNumber(); }

    void bookPlaces(int places) {
        if (places <= 0) throw InvalidDataException("количество мест должно быть положительным");
        if (places > freePlaces()) throw NoSeatsAvailableException("экскурсия " + title_, places, freePlaces());
        booked_ += places;
    }

    void cancelPlaces(int places) {
        if (places <= 0 || places > booked_) throw InvalidDataException("нельзя отменить столько мест");
        booked_ -= places;
    }

    Money totalCost(int people) const {
        Money total = price_.multiply(people);
        return people >= 5 ? total.percentOf(90.0) : total;
    }

    const std::string& getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const Attraction& getAttraction() const { return attraction_; }
    const Guide& getGuide() const { return guide_; }
    const Date& getDate() const { return date_; }
    int getCapacity() const { return capacity_; }
    int getBooked() const { return booked_; }
    const Money& getPrice() const { return price_; }
};
