#pragma once
#include <string>
#include "Date.h"

class DateRange {
    Date start_;
    Date end_;

public:
    DateRange() = default;
    DateRange(Date start, Date end) : start_(start), end_(end) {
        if (end_.isBefore(start_)) {
            throw InvalidDateRangeException("конец " + end_.toString() + " раньше начала " + start_.toString());
        }
    }

    int nights() const { return start_.daysUntil(end_); }
    bool contains(const Date& date) const { return !date.isBefore(start_) && !end_.isBefore(date); }

    bool overlaps(const DateRange& other) const { return !end_.isBefore(other.start_) && !other.end_.isBefore(start_); }

    bool startsWithin(const Date& today, int days) const {
        int left = today.daysUntil(start_);
        return left >= 0 && left <= days;
    }

    DateRange extend(int days) const { return DateRange(start_, end_.addDays(days)); }
    std::string toString() const { return start_.toString() + " — " + end_.toString(); }

    const Date& getStart() const { return start_; }
    const Date& getEnd() const { return end_; }
};
