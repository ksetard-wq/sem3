#pragma once
#include "Money.h"

class PricingPolicy {
    double markupPercent_ = 35.0;
    double minMarginPercent_ = 10.0;
    double seasonalDiscountPercent_ = 0.0;

public:
    PricingPolicy() = default;
    PricingPolicy(double markupPercent, double minMarginPercent, double seasonalDiscountPercent)
        : markupPercent_(markupPercent), minMarginPercent_(minMarginPercent),
          seasonalDiscountPercent_(seasonalDiscountPercent) {}

    Money retailPrice(const Money& cost) const { return cost.percentOf(100.0 + markupPercent_); }

    bool isMarginOk(const Money& cost, const Money& price) const {
        if (cost.isZero()) return true;
        double margin = 100.0 * static_cast<double>(price.getCents() - cost.getCents()) / static_cast<double>(price.getCents());
        return margin >= minMarginPercent_;
    }

    Money applySeasonal(const Money& price) const { return price.percentOf(100.0 - seasonalDiscountPercent_); }

    double bulkDiscountPercent(int quantity) const {
        if (quantity >= 100) return 10.0;
        if (quantity >= 50) return 5.0;
        if (quantity >= 10) return 2.0;
        return 0.0;
    }

    void startSeasonalSale(double percent) { seasonalDiscountPercent_ = percent; }

    double getMarkupPercent() const { return markupPercent_; }
    double getMinMarginPercent() const { return minMarginPercent_; }
    double getSeasonalDiscountPercent() const { return seasonalDiscountPercent_; }
};
