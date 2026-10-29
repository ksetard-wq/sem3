#pragma once
#include <vector>
#include "Booking.h"
#include "Payment.h"

class PaymentProcessor {
    std::vector<Payment> payments_;
    double feePercent_ = 2.5;
    int nextId_ = 1;
    Money dailyLimit_;

public:
    PaymentProcessor() : dailyLimit_(Money::fromUnits(1000000)) {}
    PaymentProcessor(double feePercent, Money dailyLimit) : feePercent_(feePercent), dailyLimit_(std::move(dailyLimit)) {}

    Payment charge(const Booking& booking, const Money& amount, PaymentMethod method, const Date& date) {
        if (amount.isGreaterThan(dailyLimit_)) throw PaymentDeclinedException("превышен лимит платежа " + dailyLimit_.toString());
        Payment payment(nextId_++, booking.getId(), amount, method, date);
        payment.authorize();
        payment.capture();
        payments_.push_back(payment);
        return payment;
    }

    Payment& findPayment(int id) {
        for (auto& p : payments_) {
            if (p.getId() == id) return p;
        }
        throw InvalidDataException("платёж не найден: " + std::to_string(id));
    }

    void refund(int paymentId, const Money& amount) { findPayment(paymentId).refund(amount); }

    std::vector<Payment> paymentsFor(const std::string& bookingId) const {
        std::vector<Payment> result;
        for (const auto& p : payments_) {
            if (p.getBookingId() == bookingId) result.push_back(p);
        }
        return result;
    }

    Money totalCollected() const {
        Money sum;
        for (const auto& p : payments_) {
            if (p.isSuccessful() || p.getStatus() == PaymentStatus::Refunded) sum = sum.add(p.getAmount()).subtract(p.getRefunded());
        }
        return sum;
    }

    Money totalRefunded() const {
        Money sum;
        for (const auto& p : payments_) sum = sum.add(p.getRefunded());
        return sum;
    }

    Money feeFor(const Money& amount) const { return amount.percentOf(feePercent_); }

    int successfulCount() const {
        int count = 0;
        for (const auto& p : payments_) {
            if (p.isSuccessful()) ++count;
        }
        return count;
    }

    const std::vector<Payment>& getPayments() const { return payments_; }
    double getFeePercent() const { return feePercent_; }
    const Money& getDailyLimit() const { return dailyLimit_; }
};
