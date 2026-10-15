#pragma once
#include "Book.h"
#include "Employee.h"
#include "Pallet.h"

class Loader : public Employee {
    int maxLiftKg_ = 25;
    bool forkliftCertified_ = false;

public:
    Loader(const Employee& base, int maxLiftKg) : Employee(base), maxLiftKg_(maxLiftKg) {}

    bool canLift(int weightGrams) const { return weightGrams <= maxLiftKg_ * 1000; }

    void loadPallet(Pallet& pallet, const Book& book, int quantity) const {
        if (!canLift(book.getWeightGrams() * quantity) && !forkliftCertified_) {
            throw AccessDeniedException(fullName(), "поднять партию вручную");
        }
        pallet.load(book, quantity);
    }

    void certifyForklift() { forkliftCertified_ = true; }
    bool isForkliftCertified() const { return forkliftCertified_; }

    std::string role() const override { return "Грузчик"; }

    int getMaxLiftKg() const { return maxLiftKg_; }
};
