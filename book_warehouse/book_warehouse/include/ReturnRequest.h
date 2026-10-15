#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "ISBN.h"
#include "Money.h"

enum class ReturnStatus { Pending, Approved, Rejected, Completed };

inline std::string toString(ReturnStatus status) {
    switch (status) {
        case ReturnStatus::Pending: return "На рассмотрении";
        case ReturnStatus::Approved: return "Одобрен";
        case ReturnStatus::Rejected: return "Отклонён";
        default: return "Завершён";
    }
}

class ReturnRequest {
    int id_ = 0;
    int orderId_ = 0;
    ISBN isbn_;
    int quantity_ = 0;
    std::string reason_;
    ReturnStatus status_ = ReturnStatus::Pending;
    Date requestedAt_;

public:
    ReturnRequest(int id, int orderId, ISBN isbn, int quantity, std::string reason, Date requestedAt)
        : id_(id), orderId_(orderId), isbn_(std::move(isbn)), quantity_(quantity), reason_(std::move(reason)),
          requestedAt_(requestedAt) {}

    void approve() {
        if (status_ != ReturnStatus::Pending) throw InvalidOrderStateException("одобрить возврат", toString(status_));
        status_ = ReturnStatus::Approved;
    }

    void reject() {
        if (status_ != ReturnStatus::Pending) throw InvalidOrderStateException("отклонить возврат", toString(status_));
        status_ = ReturnStatus::Rejected;
    }

    void complete() {
        if (status_ != ReturnStatus::Approved) throw InvalidOrderStateException("завершить возврат", toString(status_));
        status_ = ReturnStatus::Completed;
    }

    bool isWithinReturnWindow(const Date& orderDate, int days) const { return orderDate.daysUntil(requestedAt_) <= days; }
    Money refundAmount(const Money& unitPrice) const { return unitPrice.multiply(quantity_); }

    int getId() const { return id_; }
    int getOrderId() const { return orderId_; }
    const ISBN& getIsbn() const { return isbn_; }
    int getQuantity() const { return quantity_; }
    const std::string& getReason() const { return reason_; }
    ReturnStatus getStatus() const { return status_; }
    const Date& getRequestedAt() const { return requestedAt_; }
};
