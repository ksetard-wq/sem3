#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "ISBN.h"

class Reservation {
    int id_ = 0;
    std::string customerId_;
    ISBN isbn_;
    int quantity_ = 0;
    Date expiresAt_;
    bool active_ = true;

public:
    Reservation(int id, std::string customerId, ISBN isbn, int quantity, Date expiresAt)
        : id_(id), customerId_(std::move(customerId)), isbn_(std::move(isbn)), quantity_(quantity), expiresAt_(expiresAt) {}

    bool isExpired(const Date& today) const { return expiresAt_.isBefore(today); }

    void extend(int days) {
        if (!active_) throw InvalidOrderStateException("продлить резерв", "резерв отменён");
        expiresAt_ = expiresAt_.addDays(days);
    }

    void cancel() { active_ = false; }
    bool matches(const ISBN& isbn) const { return active_ && isbn_.equals(isbn); }

    int getId() const { return id_; }
    const std::string& getCustomerId() const { return customerId_; }
    const ISBN& getIsbn() const { return isbn_; }
    int getQuantity() const { return quantity_; }
    const Date& getExpiresAt() const { return expiresAt_; }
    bool getActive() const { return active_; }
};
