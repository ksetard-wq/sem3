#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "DateRange.h"
#include "Money.h"

enum class RoomType { Single, Double, Suite, Family };

inline std::string toString(RoomType type) {
    switch (type) {
        case RoomType::Single: return "Одноместный";
        case RoomType::Double: return "Двухместный";
        case RoomType::Suite: return "Люкс";
        default: return "Семейный";
    }
}

class Room {
    int number_ = 0;
    RoomType type_ = RoomType::Single;
    int capacity_ = 1;
    Money pricePerNight_;
    std::vector<DateRange> bookedRanges_;

public:
    Room(int number, RoomType type, int capacity, Money pricePerNight)
        : number_(number), type_(type), capacity_(capacity), pricePerNight_(std::move(pricePerNight)) {
        if (capacity_ < 1) throw InvalidDataException("вместимость номера");
    }

    bool isAvailable(const DateRange& period) const {
        for (const auto& r : bookedRanges_) {
            if (r.overlaps(period)) return false;
        }
        return true;
    }

    void book(const DateRange& period) {
        if (!isAvailable(period)) {
            throw RoomUnavailableException("№" + std::to_string(number_) + " занят на " + period.toString());
        }
        bookedRanges_.push_back(period);
    }

    void cancel(const DateRange& period) {
        auto it = std::find_if(bookedRanges_.begin(), bookedRanges_.end(), [&](const DateRange& r) {
            return r.getStart().toDayNumber() == period.getStart().toDayNumber() &&
                   r.getEnd().toDayNumber() == period.getEnd().toDayNumber();
        });
        if (it == bookedRanges_.end()) throw InvalidDataException("бронь номера " + std::to_string(number_) + " не найдена");
        bookedRanges_.erase(it);
    }

    Money costFor(const DateRange& period) const { return pricePerNight_.multiply(period.nights() < 1 ? 1 : period.nights()); }
    bool fits(int guests) const { return guests <= capacity_; }

    int nightsBooked() const {
        int sum = 0;
        for (const auto& r : bookedRanges_) sum += r.nights();
        return sum;
    }

    int getNumber() const { return number_; }
    RoomType getType() const { return type_; }
    int getCapacity() const { return capacity_; }
    const Money& getPricePerNight() const { return pricePerNight_; }
};
