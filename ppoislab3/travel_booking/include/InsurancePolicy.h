#pragma once
#include <string>
#include <vector>
#include "DateRange.h"
#include "InsuranceClaim.h"
#include "utils.h"

enum class InsuranceType { Basic, Extended, Premium };

inline std::string toString(InsuranceType type) {
    switch (type) {
        case InsuranceType::Basic: return "Базовая";
        case InsuranceType::Extended: return "Расширенная";
        default: return "Премиум";
    }
}

class InsurancePolicy {
    std::string id_;
    std::string bookingId_;
    InsuranceType type_ = InsuranceType::Basic;
    Money coverage_;
    Money premium_;
    DateRange period_;
    std::vector<InsuranceClaim> claims_;

public:
    InsurancePolicy(std::string id, std::string bookingId, InsuranceType type, Money coverage, Money premium, DateRange period)
        : id_(std::move(id)), bookingId_(std::move(bookingId)), type_(type), coverage_(std::move(coverage)),
          premium_(std::move(premium)), period_(period) {}

    static Money premiumFor(const Money& tripPrice, InsuranceType type) {
        double percent = type == InsuranceType::Basic ? 3.0 : type == InsuranceType::Extended ? 5.0 : 8.0;
        return tripPrice.percentOf(percent);
    }

    bool isActiveOn(const Date& date) const { return period_.contains(date); }

    bool covers(const std::string& reason) const {
        if (type_ == InsuranceType::Premium) return true;
        if (containsIgnoreCase(reason, "болезн")) return true;
        return type_ == InsuranceType::Extended && containsIgnoreCase(reason, "отмен");
    }

    Money remainingCoverage() const {
        Money left = coverage_;
        for (const auto& c : claims_) {
            if (c.getStatus() == ClaimStatus::Approved) left = left.subtract(c.getAmount());
        }
        return left;
    }

    void fileClaim(const InsuranceClaim& claim, const Date& today) {
        if (!isActiveOn(today)) throw InsuranceClaimException("полис " + id_ + " не действует на " + today.toString());
        if (!covers(claim.getReason())) throw InsuranceClaimException("случай \"" + claim.getReason() + "\" не покрывается типом " + toString(type_));
        if (claim.getAmount().isGreaterThan(remainingCoverage())) throw InsuranceClaimException("сумма превышает остаток покрытия");
        claims_.push_back(claim);
    }

    void approveClaim(const std::string& claimId) {
        for (auto& c : claims_) {
            if (c.getId() == claimId) {
                c.approve();
                return;
            }
        }
        throw InsuranceClaimException("заявка не найдена: " + claimId);
    }

    Money totalPaidOut() const { return coverage_.subtract(remainingCoverage()); }

    const std::string& getId() const { return id_; }
    const std::string& getBookingId() const { return bookingId_; }
    InsuranceType getType() const { return type_; }
    const Money& getCoverage() const { return coverage_; }
    const Money& getPremium() const { return premium_; }
    const DateRange& getPeriod() const { return period_; }
    const std::vector<InsuranceClaim>& getClaims() const { return claims_; }
};
