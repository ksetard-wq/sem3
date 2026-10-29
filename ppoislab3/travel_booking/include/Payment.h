#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"

enum class PaymentMethod { Card, Cash, Transfer, Wallet };
enum class PaymentStatus { Pending, Authorized, Captured, Declined, Refunded };

inline std::string toString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::Card: return "Карта";
        case PaymentMethod::Cash: return "Наличные";
        case PaymentMethod::Transfer: return "Перевод";
        default: return "Кошелёк";
    }
}

inline std::string toString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::Pending: return "Ожидает";
        case PaymentStatus::Authorized: return "Авторизован";
        case PaymentStatus::Captured: return "Проведён";
        case PaymentStatus::Declined: return "Отклонён";
        default: return "Возвращён";
    }
}

class Payment {
    int id_ = 0;
    std::string bookingId_;
    Money amount_;
    PaymentMethod method_ = PaymentMethod::Card;
    PaymentStatus status_ = PaymentStatus::Pending;
    Date createdAt_;
    Money refunded_;

public:
    Payment(int id, std::string bookingId, Money amount, PaymentMethod method, Date createdAt)
        : id_(id), bookingId_(std::move(bookingId)), amount_(std::move(amount)), method_(method), createdAt_(createdAt) {}

    void authorize() {
        if (status_ != PaymentStatus::Pending) throw InvalidStateException("авторизовать платёж", toString(status_));
        if (amount_.isZero() || amount_.isNegative()) {
            status_ = PaymentStatus::Declined;
            throw PaymentDeclinedException("сумма платежа должна быть положительной");
        }
        status_ = PaymentStatus::Authorized;
    }

    void capture() {
        if (status_ != PaymentStatus::Authorized) throw InvalidStateException("провести платёж", toString(status_));
        status_ = PaymentStatus::Captured;
    }

    void decline() { status_ = PaymentStatus::Declined; }

    void refund(const Money& part) {
        if (status_ != PaymentStatus::Captured) throw InvalidStateException("вернуть платёж", toString(status_));
        if (part.isNegative() || part.isZero() || part.isGreaterThan(refundableAmount())) {
            throw PaymentDeclinedException("недопустимая сумма возврата " + part.toString());
        }
        refunded_ = refunded_.add(part);
        if (refundableAmount().isZero()) status_ = PaymentStatus::Refunded;
    }

    Money refundableAmount() const { return amount_.subtract(refunded_); }
    bool isSuccessful() const { return status_ == PaymentStatus::Captured; }
    Money fee(double percent) const { return amount_.percentOf(percent); }

    int getId() const { return id_; }
    const std::string& getBookingId() const { return bookingId_; }
    const Money& getAmount() const { return amount_; }
    PaymentMethod getMethod() const { return method_; }
    PaymentStatus getStatus() const { return status_; }
    const Date& getCreatedAt() const { return createdAt_; }
    const Money& getRefunded() const { return refunded_; }
};
