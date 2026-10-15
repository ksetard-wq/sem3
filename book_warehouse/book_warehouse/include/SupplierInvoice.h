#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"
#include "Payment.h"

class SupplierInvoice {
    std::string id_;
    int purchaseOrderId_ = 0;
    Money amount_;
    Date issueDate_;
    Date dueDate_;
    bool paid_ = false;

public:
    SupplierInvoice(std::string id, int purchaseOrderId, Money amount, Date issueDate, Date dueDate)
        : id_(std::move(id)), purchaseOrderId_(purchaseOrderId), amount_(std::move(amount)),
          issueDate_(issueDate), dueDate_(dueDate) {}

    bool isOverdue(const Date& today) const { return !paid_ && dueDate_.isBefore(today); }
    int daysUntilDue(const Date& today) const { return today.daysUntil(dueDate_); }

    Money penalty(const Date& today, double dailyPercent) const {
        if (!isOverdue(today)) return Money();
        return amount_.percentOf(dailyPercent * dueDate_.daysUntil(today));
    }

    void settle(const Payment& payment) {
        if (paid_) throw InvalidOrderStateException("оплата счёта", "счёт уже оплачен");
        if (!payment.isSuccessful()) throw PaymentFailedException("платёж не выполнен");
        if (amount_.isGreaterThan(payment.getAmount())) throw PaymentFailedException("сумма платежа меньше суммы счёта");
        paid_ = true;
    }

    const std::string& getId() const { return id_; }
    int getPurchaseOrderId() const { return purchaseOrderId_; }
    const Money& getAmount() const { return amount_; }
    const Date& getIssueDate() const { return issueDate_; }
    const Date& getDueDate() const { return dueDate_; }
    bool getPaid() const { return paid_; }
};
