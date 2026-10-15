#pragma once
#include <utility>
#include "ISBN.h"
#include "Money.h"

class AuditDiscrepancy {
    ISBN isbn_;
    int expected_ = 0;
    int actual_ = 0;

public:
    AuditDiscrepancy(ISBN isbn, int expected, int actual) : isbn_(std::move(isbn)), expected_(expected), actual_(actual) {}

    int difference() const { return actual_ - expected_; }
    bool isShortage() const { return actual_ < expected_; }
    bool isSurplus() const { return actual_ > expected_; }
    Money valueImpact(const Money& unitPrice) const { return unitPrice.multiply(difference()); }

    const ISBN& getIsbn() const { return isbn_; }
    int getExpected() const { return expected_; }
    int getActual() const { return actual_; }
};
