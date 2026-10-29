#pragma once
#include <string>
#include <utility>
#include "DateRange.h"
#include "Money.h"

enum class BookingStatus { Pending, Confirmed, Paid, Cancelled, Completed };

inline std::string toString(BookingStatus status) {
    switch (status) {
        case BookingStatus::Pending: return "Ожидает";
        case BookingStatus::Confirmed: return "Подтверждено";
        case BookingStatus::Paid: return "Оплачено";
        case BookingStatus::Cancelled: return "Отменено";
        default: return "Завершено";
    }
}


class Booking {
    std::string id_;
    std::string travelerId_;
    Date createdAt_;
    BookingStatus status_ = BookingStatus::Pending;
    Money price_;

protected:
    void setPrice(const Money& price) { price_ = price; }

public:
    Booking(std::string id, std::string travelerId, Date createdAt, Money price)
        : id_(std::move(id)), travelerId_(std::move(travelerId)), createdAt_(createdAt), price_(std::move(price)) {
        if (price_.isNegative()) throw InvalidDataException("цена бронирования отрицательна");
    }
    virtual ~Booking() = default;

    virtual std::string type() const { return "Бронирование"; }
    virtual std::string describe() const { return type() + " " + id_; }
    virtual DateRange period() const { return DateRange(createdAt_, createdAt_); }
    virtual Money cancellationFee(const Date&) const { return price_.percentOf(10.0); }
    virtual bool isExclusive() const { return false; }
    virtual std::string destination() const { return ""; }

    void requireStatus(BookingStatus expected, const std::string& action) const {
        if (status_ != expected) throw InvalidStateException(action, toString(status_));
    }

    void confirm() {
        requireStatus(BookingStatus::Pending, "подтвердить бронирование");
        status_ = BookingStatus::Confirmed;
    }

    void markPaid() {
        requireStatus(BookingStatus::Confirmed, "оплатить бронирование");
        status_ = BookingStatus::Paid;
    }

    Money cancel(const Date& today) {
        if (status_ == BookingStatus::Cancelled || status_ == BookingStatus::Completed) {
            throw InvalidStateException("отменить бронирование", toString(status_));
        }
        Money fee = cancellationFee(today);
        status_ = BookingStatus::Cancelled;
        return fee;
    }

    void complete(const Date& today) {
        requireStatus(BookingStatus::Paid, "завершить бронирование");
        if (!period().getEnd().isBefore(today)) throw InvalidStateException("завершить бронирование", "поездка ещё не закончилась");
        status_ = BookingStatus::Completed;
    }

    bool isActive() const { return status_ != BookingStatus::Cancelled && status_ != BookingStatus::Completed; }

    Money refundAmount(const Date& today) const {
        if (status_ != BookingStatus::Paid && status_ != BookingStatus::Confirmed) return Money();
        return price_.subtract(cancellationFee(today));
    }

    const std::string& getId() const { return id_; }
    const std::string& getTravelerId() const { return travelerId_; }
    const Date& getCreatedAt() const { return createdAt_; }
    BookingStatus getStatus() const { return status_; }
    const Money& getPrice() const { return price_; }
};
