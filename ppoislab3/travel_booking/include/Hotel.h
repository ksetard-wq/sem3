#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Address.h"
#include "City.h"
#include "Room.h"
#include "utils.h"

class Hotel {
    std::string id_;
    std::string name_;
    City city_;
    Address address_;
    int stars_ = 3;
    std::vector<Room> rooms_;
    std::vector<std::string> amenities_;
    double rating_ = 4.0;

public:
    Hotel(std::string id, std::string name, City city, Address address, int stars)
        : id_(std::move(id)), name_(std::move(name)), city_(std::move(city)), address_(std::move(address)), stars_(stars) {
        if (stars < 1 || stars > 5) throw InvalidDataException("звёзды отеля от 1 до 5");
    }

    void addRoom(const Room& room) {
        for (const auto& r : rooms_) {
            if (r.getNumber() == room.getNumber()) throw DuplicateEntityException("номер " + std::to_string(room.getNumber()));
        }
        rooms_.push_back(room);
    }

    void addAmenity(const std::string& amenity) {
        if (!hasAmenity(amenity)) amenities_.push_back(amenity);
    }

    bool hasAmenity(const std::string& amenity) const {
        for (const auto& a : amenities_) {
            if (containsIgnoreCase(a, amenity)) return true;
        }
        return false;
    }

    Room* findRoom(RoomType type, int guests, const DateRange& period) {
        for (auto& r : rooms_) {
            if (r.getType() == type && r.fits(guests) && r.isAvailable(period)) return &r;
        }
        return nullptr;
    }

    int bookRoom(RoomType type, int guests, const DateRange& period) {
        Room* room = findRoom(type, guests, period);
        if (room == nullptr) throw RoomUnavailableException(name_ + ": нет свободного номера типа \"" + toString(type) + "\"");
        room->book(period);
        return room->getNumber();
    }

    void cancelRoom(int number, const DateRange& period) {
        for (auto& r : rooms_) {
            if (r.getNumber() == number) {
                r.cancel(period);
                return;
            }
        }
        throw InvalidDataException("номер " + std::to_string(number) + " не найден в отеле " + name_);
    }

    int availableRoomsCount(const DateRange& period) const {
        int count = 0;
        for (const auto& r : rooms_) {
            if (r.isAvailable(period)) ++count;
        }
        return count;
    }

    Money averagePrice() const {
        if (rooms_.empty()) return Money();
        Money sum;
        for (const auto& r : rooms_) sum = sum.add(r.getPricePerNight());
        return sum.divide(static_cast<int>(rooms_.size()));
    }

    void updateRating(double rating) {
        if (rating < 0.0 || rating > 10.0) throw InvalidDataException("рейтинг отеля от 0 до 10");
        rating_ = rating;
    }

    bool isLuxury() const { return stars_ >= 5; }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const City& getCity() const { return city_; }
    const Address& getAddress() const { return address_; }
    int getStars() const { return stars_; }
    const std::vector<Room>& getRooms() const { return rooms_; }
    double getRating() const { return rating_; }
};
