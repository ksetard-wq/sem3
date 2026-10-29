#pragma once
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "Booking.h"

class TravelAnalytics {
    std::string title_;
    Date generatedAt_;

public:
    TravelAnalytics(std::string title, Date generatedAt) : title_(std::move(title)), generatedAt_(generatedAt) {}

    Money revenue(const std::vector<std::shared_ptr<Booking>>& bookings) const {
        Money sum;
        for (const auto& b : bookings) {
            if (b->getStatus() == BookingStatus::Paid || b->getStatus() == BookingStatus::Completed) sum = sum.add(b->getPrice());
        }
        return sum;
    }

    double cancellationRate(const std::vector<std::shared_ptr<Booking>>& bookings) const {
        if (bookings.empty()) return 0.0;
        int cancelled = 0;
        for (const auto& b : bookings) {
            if (b->getStatus() == BookingStatus::Cancelled) ++cancelled;
        }
        return 100.0 * cancelled / static_cast<double>(bookings.size());
    }

    Money averageBookingValue(const std::vector<std::shared_ptr<Booking>>& bookings) const {
        Money sum;
        int count = 0;
        for (const auto& b : bookings) {
            if (b->getStatus() != BookingStatus::Cancelled) {
                sum = sum.add(b->getPrice());
                ++count;
            }
        }
        return count == 0 ? Money() : sum.divide(count);
    }

    int countByType(const std::vector<std::shared_ptr<Booking>>& bookings, const std::string& type) const {
        int count = 0;
        for (const auto& b : bookings) {
            if (b->type() == type) ++count;
        }
        return count;
    }

    std::string mostPopularDestination(const std::vector<std::shared_ptr<Booking>>& bookings) const {
        std::map<std::string, int> counts;
        for (const auto& b : bookings) {
            if (!b->destination().empty() && b->getStatus() != BookingStatus::Cancelled) ++counts[b->destination()];
        }
        std::string best;
        int bestCount = 0;
        for (const auto& entry : counts) {
            if (entry.second > bestCount) {
                best = entry.first;
                bestCount = entry.second;
            }
        }
        return best;
    }

    std::string summary(const std::vector<std::shared_ptr<Booking>>& bookings) const {
        std::ostringstream out;
        out << "=== " << title_ << " (" << generatedAt_.toString() << ") ===\n"
            << "Бронирований: " << bookings.size() << ", выручка: " << revenue(bookings).toString() << "\n"
            << "Средний чек: " << averageBookingValue(bookings).toString() << ", доля отмен: " << cancellationRate(bookings) << "%\n"
            << "Авиабилетов: " << countByType(bookings, "Авиабилет") << ", отелей: " << countByType(bookings, "Отель")
            << ", туров: " << countByType(bookings, "Тур") << "\n"
            << "Самое популярное направление: " << (mostPopularDestination(bookings).empty() ? "—" : mostPopularDestination(bookings)) << "\n";
        return out.str();
    }

    const std::string& getTitle() const { return title_; }
    const Date& getGeneratedAt() const { return generatedAt_; }
};
