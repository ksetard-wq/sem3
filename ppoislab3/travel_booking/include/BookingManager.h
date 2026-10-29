#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Booking.h"

class BookingManager {
    std::map<std::string, std::shared_ptr<Booking>> bookings_;
    int nextNumber_ = 1;
    Money cancelledFees_;

public:
    std::string generateId(const std::string& prefix) { return prefix + "-" + std::to_string(nextNumber_++); }

    void add(const std::shared_ptr<Booking>& booking) {
        if (!booking) throw InvalidDataException("пустое бронирование");
        if (!bookings_.emplace(booking->getId(), booking).second) throw DuplicateEntityException("бронь " + booking->getId());
    }

    std::shared_ptr<Booking> find(const std::string& id) const {
        auto it = bookings_.find(id);
        if (it == bookings_.end()) throw BookingNotFoundException(id);
        return it->second;
    }

    Money cancel(const std::string& id, const Date& today) {
        Money fee = find(id)->cancel(today);
        cancelledFees_ = cancelledFees_.add(fee);
        return fee;
    }

    std::vector<std::shared_ptr<Booking>> activeFor(const std::string& travelerId) const {
        std::vector<std::shared_ptr<Booking>> result;
        for (const auto& entry : bookings_) {
            if (entry.second->getTravelerId() == travelerId && entry.second->isActive()) result.push_back(entry.second);
        }
        return result;
    }

    int countByStatus(BookingStatus status) const {
        int count = 0;
        for (const auto& entry : bookings_) {
            if (entry.second->getStatus() == status) ++count;
        }
        return count;
    }

    Money revenue() const {
        Money sum;
        for (const auto& entry : bookings_) {
            if (entry.second->getStatus() == BookingStatus::Paid || entry.second->getStatus() == BookingStatus::Completed) {
                sum = sum.add(entry.second->getPrice());
            }
        }
        return sum;
    }

    std::vector<std::shared_ptr<Booking>> all() const {
        std::vector<std::shared_ptr<Booking>> result;
        for (const auto& entry : bookings_) result.push_back(entry.second);
        return result;
    }

    size_t size() const { return bookings_.size(); }

    const Money& getCancelledFees() const { return cancelledFees_; }
};
