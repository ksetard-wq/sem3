#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "ISBN.h"

enum class MovementType { Receipt, Issue, WriteOff };

inline std::string toString(MovementType type) {
    switch (type) {
        case MovementType::Receipt: return "Приход";
        case MovementType::Issue: return "Расход";
        default: return "Списание";
    }
}

class StockMovement {
    int id_ = 0;
    MovementType type_ = MovementType::Receipt;
    ISBN isbn_;
    int quantity_ = 0;
    Date date_;
    std::string employeeId_;

public:
    StockMovement(int id, MovementType type, ISBN isbn, int quantity, Date date, std::string employeeId)
        : id_(id), type_(type), isbn_(std::move(isbn)), quantity_(quantity), date_(date),
          employeeId_(std::move(employeeId)) {}

    bool isInbound() const { return type_ == MovementType::Receipt; }
    bool isOutbound() const { return type_ != MovementType::Receipt; }
    int signedQuantity() const { return isInbound() ? quantity_ : -quantity_; }

    std::string describe() const {
        return date_.toString() + " " + toString(type_) + " " + isbn_.formatted() + " x" + std::to_string(quantity_) +
               " (" + employeeId_ + ")";
    }

    int getId() const { return id_; }
    MovementType getType() const { return type_; }
    const ISBN& getIsbn() const { return isbn_; }
    int getQuantity() const { return quantity_; }
    const Date& getDate() const { return date_; }
    const std::string& getEmployeeId() const { return employeeId_; }
};
