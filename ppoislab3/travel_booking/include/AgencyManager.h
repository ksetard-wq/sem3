#pragma once
#include "Staff.h"
#include "UserAccount.h"

class AgencyManager : public Staff {
    Money approvalLimit_;
    int approvedRefunds_ = 0;

public:
    AgencyManager(const Staff& base, Money approvalLimit) : Staff(base), approvalLimit_(std::move(approvalLimit)) {}

    bool canApproveRefund(const Money& amount) const { return !amount.isGreaterThan(approvalLimit_); }

    void approveRefund(const Money& amount) {
        if (!canApproveRefund(amount)) throw AccessDeniedException(fullName(), "одобрить возврат " + amount.toString());
        ++approvedRefunds_;
    }

    void unlockAccount(UserAccount& account) const { account.unlock(); }
    bool approveDiscount(double percent) const { return percent > 0.0 && percent <= 30.0; }

    std::string role() const override { return "Руководитель агентства"; }
    bool canRefund() const override { return true; }

    const Money& getApprovalLimit() const { return approvalLimit_; }
    int getApprovedRefunds() const { return approvedRefunds_; }
};
