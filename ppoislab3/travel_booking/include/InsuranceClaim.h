#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"

enum class ClaimStatus { Pending, Approved, Rejected };

inline std::string toString(ClaimStatus status) {
    switch (status) {
        case ClaimStatus::Pending: return "На рассмотрении";
        case ClaimStatus::Approved: return "Одобрена";
        default: return "Отклонена";
    }
}

class InsuranceClaim {
    std::string id_;
    Money amount_;
    std::string reason_;
    Date filedAt_;
    ClaimStatus status_ = ClaimStatus::Pending;

public:
    InsuranceClaim(std::string id, Money amount, std::string reason, Date filedAt)
        : id_(std::move(id)), amount_(std::move(amount)), reason_(std::move(reason)), filedAt_(filedAt) {
        if (amount_.isNegative() || amount_.isZero()) throw InvalidDataException("сумма страхового случая должна быть положительной");
    }

    void approve() {
        if (status_ != ClaimStatus::Pending) throw InvalidStateException("одобрить заявку", toString(status_));
        status_ = ClaimStatus::Approved;
    }

    void reject() {
        if (status_ != ClaimStatus::Pending) throw InvalidStateException("отклонить заявку", toString(status_));
        status_ = ClaimStatus::Rejected;
    }

    bool isPending() const { return status_ == ClaimStatus::Pending; }
    Money payout(const Money& remainingCoverage) const { return amount_.isGreaterThan(remainingCoverage) ? remainingCoverage : amount_; }

    const std::string& getId() const { return id_; }
    const Money& getAmount() const { return amount_; }
    const std::string& getReason() const { return reason_; }
    const Date& getFiledAt() const { return filedAt_; }
    ClaimStatus getStatus() const { return status_; }
};
