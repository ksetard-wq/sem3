#pragma once
#include <string>
#include <vector>
#include "Attraction.h"
#include "Flight.h"
#include "Hotel.h"
#include "TourPackage.h"
#include "TravelAdvisory.h"

class TravelCatalog {
    std::vector<Flight> flights_;
    std::vector<Hotel> hotels_;
    std::vector<TourPackage> tours_;
    std::vector<Attraction> attractions_;
    std::vector<TravelAdvisory> advisories_;

public:
    void addFlight(const Flight& flight) {
        for (const auto& f : flights_) {
            if (f.getNumber() == flight.getNumber() && f.getDeparture().toDayNumber() == flight.getDeparture().toDayNumber()) {
                throw DuplicateEntityException("рейс " + flight.getNumber());
            }
        }
        flights_.push_back(flight);
    }

    void addHotel(const Hotel& hotel) {
        for (const auto& h : hotels_) {
            if (h.getId() == hotel.getId()) throw DuplicateEntityException("отель " + hotel.getId());
        }
        hotels_.push_back(hotel);
    }

    void addTour(const TourPackage& tour) {
        for (const auto& t : tours_) {
            if (t.getId() == tour.getId()) throw DuplicateEntityException("тур " + tour.getId());
        }
        tours_.push_back(tour);
    }

    void addAttraction(const Attraction& attraction) { attractions_.push_back(attraction); }

    void addAdvisory(const TravelAdvisory& advisory) {
        for (auto& a : advisories_) {
            if (a.getCountry().isSameCountry(advisory.getCountry())) {
                a = advisory;
                return;
            }
        }
        advisories_.push_back(advisory);
    }

    Flight& findFlight(const std::string& number) {
        for (auto& f : flights_) {
            if (f.getNumber() == number) return f;
        }
        throw FlightNotFoundException(number);
    }

    Hotel& findHotel(const std::string& id) {
        for (auto& h : hotels_) {
            if (h.getId() == id) return h;
        }
        throw InvalidDataException("отель не найден: " + id);
    }

    TourPackage& findTour(const std::string& id) {
        for (auto& t : tours_) {
            if (t.getId() == id) return t;
        }
        throw InvalidDataException("тур не найден: " + id);
    }

    std::vector<Flight> searchFlights(const City& from, const City& to, const Date& date) const {
        std::vector<Flight> result;
        for (const auto& f : flights_) {
            if (f.getOrigin().getCity().getId() == from.getId() && f.getDestination().getCity().getId() == to.getId() &&
                f.departsOn(date) && !f.isFull()) {
                result.push_back(f);
            }
        }
        return result;
    }

    std::string cheapestFlightNumber(const City& from, const City& to, const Date& date, CabinClass cabin) const {
        const Flight* best = nullptr;
        std::vector<Flight> found = searchFlights(from, to, date);
        for (const auto& f : found) {
            if (!f.hasFreeSeat(cabin)) continue;
            if (best == nullptr || best->priceFor(cabin).isGreaterThan(f.priceFor(cabin))) best = &f;
        }
        if (best == nullptr) throw FlightNotFoundException(from.getName() + " -> " + to.getName() + " на " + date.toString());
        return best->getNumber();
    }

    std::vector<Hotel> findHotels(const City& city, const DateRange& stay, int guests) const {
        std::vector<Hotel> result;
        for (const auto& h : hotels_) {
            if (h.getCity().getId() != city.getId()) continue;
            for (const auto& r : h.getRooms()) {
                if (r.fits(guests) && r.isAvailable(stay)) {
                    result.push_back(h);
                    break;
                }
            }
        }
        return result;
    }

    std::vector<TourPackage> toursTo(const City& city) const {
        std::vector<TourPackage> result;
        for (const auto& t : tours_) {
            if (t.getDestination().getId() == city.getId() && t.freePlaces() > 0) result.push_back(t);
        }
        return result;
    }

    std::vector<Attraction> attractionsIn(const City& city) const {
        std::vector<Attraction> result;
        for (const auto& a : attractions_) {
            if (a.isInCity(city)) result.push_back(a);
        }
        return result;
    }

    const TravelAdvisory* advisoryFor(const Country& country) const {
        for (const auto& a : advisories_) {
            if (a.getCountry().isSameCountry(country)) return &a;
        }
        return nullptr;
    }

    const std::vector<Flight>& getFlights() const { return flights_; }
    const std::vector<Hotel>& getHotels() const { return hotels_; }
    const std::vector<TourPackage>& getTours() const { return tours_; }
    const std::vector<Attraction>& getAttractions() const { return attractions_; }
    const std::vector<TravelAdvisory>& getAdvisories() const { return advisories_; }
};
