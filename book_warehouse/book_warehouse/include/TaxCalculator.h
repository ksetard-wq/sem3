#pragma once
#include "Money.h"

class TaxCalculator {
    double standardVat_ = 20.0;
    double reducedVat_ = 10.0;
    double incomeTaxPercent_ = 13.0;

public:
    TaxCalculator() = default;

    Money vatFor(const Money& netAmount, bool isBook) const {
        return netAmount.percentOf(isBook ? reducedVat_ : standardVat_);
    }

    Money priceWithVat(const Money& netAmount, bool isBook) const { return netAmount.add(vatFor(netAmount, isBook)); }

    Money vatIncluded(const Money& grossAmount, bool isBook) const {
        double rate = isBook ? reducedVat_ : standardVat_;
        return grossAmount.percentOf(rate / (100.0 + rate) * 100.0);
    }

    Money incomeTax(const Money& grossSalary) const { return grossSalary.percentOf(incomeTaxPercent_); }

    double getStandardVat() const { return standardVat_; }
    double getReducedVat() const { return reducedVat_; }
    double getIncomeTaxPercent() const { return incomeTaxPercent_; }
};
