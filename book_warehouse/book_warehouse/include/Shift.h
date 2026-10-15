#pragma once
#include <string>
#include <utility>
#include "Date.h"

class Shift {
    std::string employeeId_;
    Date date_;
    int startHour_ = 0;
    int endHour_ = 0;

public:
    Shift(std::string employeeId, Date date, int startHour, int endHour)
        : employeeId_(std::move(employeeId)), date_(date), startHour_(startHour), endHour_(endHour) {
        if (startHour < 0 || endHour > 24 || startHour >= endHour) throw InvalidDataException("границы смены");
    }

    int durationHours() const { return endHour_ - startHour_; }
    bool coversHour(int hour) const { return hour >= startHour_ && hour < endHour_; }
    bool isNightShift() const { return startHour_ >= 22 || endHour_ <= 6; }

    bool overlaps(const Shift& other) const {
        return date_.toDayNumber() == other.date_.toDayNumber() && startHour_ < other.endHour_ &&
               other.startHour_ < endHour_;
    }

    const std::string& getEmployeeId() const { return employeeId_; }
    const Date& getDate() const { return date_; }
    int getStartHour() const { return startHour_; }
    int getEndHour() const { return endHour_; }
};
