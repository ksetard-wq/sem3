#pragma once
#include <string>
#include <utility>
#include "Money.h"

class PayrollRecord {
    std::string employeeId_;
    std::string period_;
    Money baseSalary_;
    Money bonus_;
    Money deductions_;
    bool paid_ = false;

public:
    PayrollRecord(std::string employeeId, std::string period, Money baseSalary, Money bonus, Money deductions)
        : employeeId_(std::move(employeeId)), period_(std::move(period)), baseSalary_(std::move(baseSalary)),
          bonus_(std::move(bonus)), deductions_(std::move(deductions)) {}

    Money gross() const { return baseSalary_.add(bonus_); }
    Money netPay() const { return gross().subtract(deductions_); }
    void applyBonus(const Money& extra) { bonus_ = bonus_.add(extra); }
    void addDeduction(const Money& extra) { deductions_ = deductions_.add(extra); }

    void markPaid() {
        if (paid_) throw InvalidOrderStateException("выплата зарплаты", "уже выплачено");
        paid_ = true;
    }

    const std::string& getEmployeeId() const { return employeeId_; }
    const std::string& getPeriod() const { return period_; }
    const Money& getBaseSalary() const { return baseSalary_; }
    const Money& getBonus() const { return bonus_; }
    const Money& getDeductions() const { return deductions_; }
    bool getPaid() const { return paid_; }
};
