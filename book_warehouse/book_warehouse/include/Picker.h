#pragma once
#include "BookCatalog.h"
#include "CustomerOrder.h"
#include "Employee.h"
#include "Package.h"

class Picker : public Employee {
    int pickedToday_ = 0;
    int pickRatePerHour_ = 60;

public:
    Picker(const Employee& base, int pickRatePerHour) : Employee(base), pickRatePerHour_(pickRatePerHour) {}

    double estimateHours(const CustomerOrder& order) const {
        return static_cast<double>(order.totalQuantity()) / pickRatePerHour_;
    }

    void pickOrder(const CustomerOrder& order, Package& package, const BookCatalog& catalog) {
        for (const auto& line : order.getLines()) {
            package.addItem(catalog.findByIsbn(line.getIsbn()), line.getQuantity());
            pickedToday_ += line.getQuantity();
        }
    }

    void resetDay() { pickedToday_ = 0; }
    bool isOverloaded(int dailyLimit) const { return pickedToday_ > dailyLimit; }

    std::string role() const override { return "Комплектовщик"; }

    int getPickedToday() const { return pickedToday_; }
    int getPickRatePerHour() const { return pickRatePerHour_; }
};
