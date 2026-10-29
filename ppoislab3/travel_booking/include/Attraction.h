#pragma once
#include <string>
#include <utility>
#include "City.h"
#include "Money.h"

class Attraction {
    std::string id_;
    std::string name_;
    City city_;
    Money ticketPrice_;
    int openHour_ = 9;
    int closeHour_ = 18;

public:
    Attraction(std::string id, std::string name, City city, Money ticketPrice, int openHour, int closeHour)
        : id_(std::move(id)), name_(std::move(name)), city_(std::move(city)), ticketPrice_(std::move(ticketPrice)),
          openHour_(openHour), closeHour_(closeHour) {
        if (openHour < 0 || closeHour > 24 || openHour >= closeHour) throw InvalidDataException("часы работы достопримечательности");
    }

    bool isOpenAt(int hour) const { return hour >= openHour_ && hour < closeHour_; }
    Money ticketsCost(int people) const { return ticketPrice_.multiply(people); }
    int openHours() const { return closeHour_ - openHour_; }
    bool isInCity(const City& city) const { return city_.getId() == city.getId(); }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const City& getCity() const { return city_; }
    const Money& getTicketPrice() const { return ticketPrice_; }
    int getOpenHour() const { return openHour_; }
    int getCloseHour() const { return closeHour_; }
};
