#pragma once
#include <utility>
#include "ISBN.h"
#include "Money.h"

class OrderLine {
    ISBN isbn_;
    int quantity_ = 0;
    Money unitPrice_;

public:
    OrderLine(ISBN isbn, int quantity, Money unitPrice)
        : isbn_(std::move(isbn)), quantity_(quantity), unitPrice_(std::move(unitPrice)) {
        if (!isValid()) throw InvalidDataException("позиция заказа");
    }

    Money total() const { return unitPrice_.multiply(quantity_); }

    void changeQuantity(int quantity) {
        if (quantity <= 0) throw InvalidDataException("количество должно быть положительным");
        quantity_ = quantity;
    }

    bool isValid() const { return quantity_ > 0 && !unitPrice_.isNegative(); }
    void applyDiscount(double percent) { unitPrice_ = unitPrice_.percentOf(100.0 - percent); }

    const ISBN& getIsbn() const { return isbn_; }
    int getQuantity() const { return quantity_; }
    const Money& getUnitPrice() const { return unitPrice_; }
};
