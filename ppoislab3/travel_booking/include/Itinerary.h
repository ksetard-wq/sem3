#pragma once
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "Booking.h"

class Itinerary {
    std::string id_;
    std::string title_;
    std::string travelerId_;
    std::vector<std::shared_ptr<Booking>> bookings_;
    Date createdAt_;

public:
    Itinerary(std::string id, std::string title, std::string travelerId, Date createdAt)
        : id_(std::move(id)), title_(std::move(title)), travelerId_(std::move(travelerId)), createdAt_(createdAt) {}

    void addBooking(const std::shared_ptr<Booking>& booking) {
        if (!booking) throw InvalidDataException("пустое бронирование");
        if (booking->getTravelerId() != travelerId_) throw AccessDeniedException(booking->getTravelerId(), "добавить бронь в чужой маршрут");
        for (const auto& b : bookings_) {
            if (b->getId() == booking->getId()) throw DuplicateEntityException("бронь " + booking->getId() + " в маршруте");
        }
        bookings_.push_back(booking);
    }

    void removeBooking(const std::string& bookingId) {
        auto it = std::remove_if(bookings_.begin(), bookings_.end(), [&](const std::shared_ptr<Booking>& b) { return b->getId() == bookingId; });
        if (it == bookings_.end()) throw BookingNotFoundException(bookingId);
        bookings_.erase(it, bookings_.end());
    }

    Money totalPrice() const {
        Money sum;
        for (const auto& b : bookings_) {
            if (b->isActive()) sum = sum.add(b->getPrice());
        }
        return sum;
    }

    // Количество пар «занимающих» бронирований (отель, тур), чьи периоды пересекаются.
    int overlappingPairs() const {
        int count = 0;
        for (size_t i = 0; i < bookings_.size(); ++i) {
            for (size_t j = i + 1; j < bookings_.size(); ++j) {
                if (bookings_[i]->isActive() && bookings_[j]->isActive() && bookings_[i]->isExclusive() &&
                    bookings_[j]->isExclusive() && bookings_[i]->period().overlaps(bookings_[j]->period())) {
                    ++count;
                }
            }
        }
        return count;
    }

    bool hasConflicts() const { return overlappingPairs() > 0; }

    DateRange totalPeriod() const {
        if (bookings_.empty()) throw InvalidStateException("определить период маршрута", "маршрут пуст");
        Date first = bookings_.front()->period().getStart();
        Date last = bookings_.front()->period().getEnd();
        for (const auto& b : bookings_) {
            if (b->period().getStart().isBefore(first)) first = b->period().getStart();
            if (last.isBefore(b->period().getEnd())) last = b->period().getEnd();
        }
        return DateRange(first, last);
    }

    std::vector<std::string> destinations() const {
        std::vector<std::string> result;
        for (const auto& b : bookings_) {
            std::string d = b->destination();
            if (!d.empty() && std::find(result.begin(), result.end(), d) == result.end()) result.push_back(d);
        }
        return result;
    }

    bool isFullyPaid() const {
        for (const auto& b : bookings_) {
            if (b->isActive() && b->getStatus() != BookingStatus::Paid) return false;
        }
        return !bookings_.empty();
    }

    const std::string& getId() const { return id_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getTravelerId() const { return travelerId_; }
    const std::vector<std::shared_ptr<Booking>>& getBookings() const { return bookings_; }
    const Date& getCreatedAt() const { return createdAt_; }
};
