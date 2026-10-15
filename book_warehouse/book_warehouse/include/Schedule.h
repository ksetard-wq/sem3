#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Shift.h"

class Schedule {
    std::vector<Shift> shifts_;
    Date weekStart_;

public:
    Schedule() = default;
    explicit Schedule(Date weekStart) : weekStart_(weekStart) {}

    void addShift(const Shift& shift) {
        for (const auto& s : shifts_) {
            if (s.getEmployeeId() == shift.getEmployeeId() && s.overlaps(shift)) {
                throw DuplicateEntityException("пересечение смен сотрудника " + shift.getEmployeeId());
            }
        }
        shifts_.push_back(shift);
    }

    std::vector<Shift> shiftsFor(const std::string& employeeId) const {
        std::vector<Shift> result;
        for (const auto& s : shifts_) {
            if (s.getEmployeeId() == employeeId) result.push_back(s);
        }
        return result;
    }

    int hoursFor(const std::string& employeeId) const {
        int sum = 0;
        for (const auto& s : shifts_) {
            if (s.getEmployeeId() == employeeId) sum += s.durationHours();
        }
        return sum;
    }

    void removeShiftsFor(const std::string& employeeId) {
        shifts_.erase(std::remove_if(shifts_.begin(), shifts_.end(),
                                     [&](const Shift& s) { return s.getEmployeeId() == employeeId; }),
                      shifts_.end());
    }

    bool isEmployeeBusy(const std::string& employeeId, const Date& date, int hour) const {
        for (const auto& s : shifts_) {
            if (s.getEmployeeId() == employeeId && s.getDate().toDayNumber() == date.toDayNumber() &&
                s.coversHour(hour)) {
                return true;
            }
        }
        return false;
    }

    const std::vector<Shift>& getShifts() const { return shifts_; }
    const Date& getWeekStart() const { return weekStart_; }
};
