#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"

enum class PaymentMethod { Cash, Card, BankTransfer, Online };
enum class PaymentStatus { Pending, Authorized, Completed, Failed, Refunded };

inline std::string toString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::Cash: return "Наличные";
        case PaymentMethod::Card: return "Карта";
        case PaymentMethod::BankTransfer: return "Банковский перевод";
        default: return "Онлайн";
    }
}

inline std::string toString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::Pending: return "Ожидает";
        case PaymentStatus::Authorized: return "Авторизован";
        case PaymentStatus::Completed: return "Выполнен";
        case PaymentStatus::Failed: return "Отклонён";
        default: return "Возвращён";
    }
}

class Payment {
    int id_ = 0;
    Money amount_;
    PaymentMethod method_ = PaymentMethod::Cash;
    PaymentStatus status_ = PaymentStatus::Pending;
    Date date_;

public:
    Payment(int id, Money amount, PaymentMethod method, Date date)
        : id_(id), amount_(std::move(amount)), method_(method), date_(date) {}

    void authorize() {
        if (status_ != PaymentStatus::Pending) throw InvalidOrderStateException("авторизация платежа", toString(status_));
        if (amount_.isZero() || amount_.isNegative()) throw PaymentFailedException("сумма платежа должна быть положительной");
        status_ = PaymentStatus::Authorized;
    }

    void complete() {
        if (status_ != PaymentStatus::Authorized) throw InvalidOrderStateException("проведение платежа", toString(status_));
        status_ = PaymentStatus::Completed;
    }

    void process() {
        authorize();
        complete();
    }

    void fail() { status_ = PaymentStatus::Failed; }

    void refund() {
        if (status_ != PaymentStatus::Completed) throw InvalidOrderStateException("возврат платежа", toString(status_));
        status_ = PaymentStatus::Refunded;
    }

    bool isSuccessful() const { return status_ == PaymentStatus::Completed; }
    Money processingFee(double percent) const { return amount_.percentOf(percent); }

    int getId() const { return id_; }
    const Money& getAmount() const { return amount_; }
    PaymentMethod getMethod() const { return method_; }
    PaymentStatus getStatus() const { return status_; }
    const Date& getDate() const { return date_; }
};
