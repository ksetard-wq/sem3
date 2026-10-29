#pragma once
#include "Booking.h"

class RefundCalculator {
    Money processingFee_;
    Money minRefund_;
    int refundWindowDays_ = 14;

public:
    RefundCalculator() : processingFee_(Money::fromUnits(100)), minRefund_(Money::fromUnits(50)) {}
    RefundCalculator(Money processingFee, Money minRefund, int refundWindowDays)
        : processingFee_(std::move(processingFee)), minRefund_(std::move(minRefund)), refundWindowDays_(refundWindowDays) {}

    bool isRefundable(const Booking& booking, const Date& today) const {
        bool statusOk = booking.getStatus() == BookingStatus::Paid || booking.getStatus() == BookingStatus::Confirmed;
        return statusOk && !booking.period().getStart().isBefore(today);
    }

    bool isWithinWindow(const Booking& booking, const Date& today) const {
        return booking.getCreatedAt().daysUntil(today) <= refundWindowDays_;
    }

    Money refundFor(const Booking& booking, const Date& today) const {
        if (!isRefundable(booking, today)) return Money();
        Money refund = booking.refundAmount(today).subtract(processingFee_);
        if (refund.isNegative() || minRefund_.isGreaterThan(refund)) return Money();
        return refund;
    }

    double refundSharePercent(const Booking& booking, const Date& today) const {
        if (booking.getPrice().isZero()) return 0.0;
        return 100.0 * static_cast<double>(refundFor(booking, today).getCents()) / static_cast<double>(booking.getPrice().getCents());
    }

    const Money& getProcessingFee() const { return processingFee_; }
    const Money& getMinRefund() const { return minRefund_; }
    int getRefundWindowDays() const { return refundWindowDays_; }
};
