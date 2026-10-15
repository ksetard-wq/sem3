#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "ISBN.h"
#include "Money.h"
#include "utils.h"

class WriteOffRecord {
    int id_ = 0;
    ISBN isbn_;
    int quantity_ = 0;
    std::string reason_;
    Date date_;
    std::string approvedBy_;

public:
    WriteOffRecord(int id, ISBN isbn, int quantity, std::string reason, Date date, std::string approvedBy)
        : id_(id), isbn_(std::move(isbn)), quantity_(quantity), reason_(std::move(reason)), date_(date),
          approvedBy_(std::move(approvedBy)) {}

    Money lossAmount(const Money& unitCost) const { return unitCost.multiply(quantity_); }
    bool isDamage() const { return containsIgnoreCase(reason_, "поврежд") || containsIgnoreCase(reason_, "вод"); }
    std::string describe() const {
        return date_.toString() + " списано " + std::to_string(quantity_) + " шт. " + isbn_.formatted() + ": " + reason_;
    }

    int getId() const { return id_; }
    const ISBN& getIsbn() const { return isbn_; }
    int getQuantity() const { return quantity_; }
    const std::string& getReason() const { return reason_; }
    const Date& getDate() const { return date_; }
    const std::string& getApprovedBy() const { return approvedBy_; }
};
