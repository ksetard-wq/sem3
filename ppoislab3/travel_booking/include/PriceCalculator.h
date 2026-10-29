#pragma once
#include "DateRange.h"
#include "Money.h"

class PriceCalculator {
    double highSeasonMultiplier_ = 1.3;
    double weekendSurchargePercent_ = 10.0;
    int groupDiscountThreshold_ = 4;
    double groupDiscountPercent_ = 8.0;

public:
    PriceCalculator() = default;
    PriceCalculator(double highSeasonMultiplier, double weekendSurchargePercent, int groupThreshold, double groupDiscountPercent)
        : highSeasonMultiplier_(highSeasonMultiplier), weekendSurchargePercent_(weekendSurchargePercent),
          groupDiscountThreshold_(groupThreshold), groupDiscountPercent_(groupDiscountPercent) {}

    bool isHighSeason(const Date& date) const { return (date.getMonth() >= 6 && date.getMonth() <= 8) || date.getMonth() == 12; }

    Money seasonalPrice(const Money& price, const Date& date) const {
        return isHighSeason(date) ? price.percentOf(100.0 * highSeasonMultiplier_) : price;
    }

    Money weekendPrice(const Money& price, const Date& date) const {
        return date.isWeekend() ? price.percentOf(100.0 + weekendSurchargePercent_) : price;
    }

    Money groupDiscount(const Money& total, int people) const {
        return people >= groupDiscountThreshold_ ? total.percentOf(100.0 - groupDiscountPercent_) : total;
    }

    // Стоимость проживания с учётом сезона и выходных по каждой ночи.
    Money totalForStay(const Money& pricePerNight, const DateRange& stay, int people) const {
        Money total;
        int nights = stay.nights() < 1 ? 1 : stay.nights();
        for (int i = 0; i < nights; ++i) {
            Date night = stay.getStart().addDays(i);
            total = total.add(seasonalPrice(weekendPrice(pricePerNight, night), night));
        }
        return groupDiscount(total, people);
    }

    double getHighSeasonMultiplier() const { return highSeasonMultiplier_; }
    double getWeekendSurchargePercent() const { return weekendSurchargePercent_; }
    int getGroupDiscountThreshold() const { return groupDiscountThreshold_; }
    double getGroupDiscountPercent() const { return groupDiscountPercent_; }
};
