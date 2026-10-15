#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"
#include "Person.h"

class Employee : public Person {
    std::string position_;
    Money salary_;
    Date hireDate_;
    bool active_ = true;

public:
    Employee() = default;
    Employee(std::string id, std::string firstName, std::string lastName, ContactInfo contact, Date birthDate,
             std::string position, Money salary, Date hireDate)
        : Person(std::move(id), std::move(firstName), std::move(lastName), std::move(contact), birthDate),
          position_(std::move(position)), salary_(std::move(salary)), hireDate_(hireDate) {}

    int yearsOfService(const Date& today) const { return hireDate_.daysUntil(today) / 365; }

    void raiseSalary(double percent) {
        if (percent <= 0) throw InvalidDataException("процент повышения должен быть положительным");
        salary_ = salary_.add(salary_.percentOf(percent));
    }

    void dismiss() { active_ = false; }
    bool isActive() const { return active_; }

    virtual std::string role() const { return "Сотрудник"; }
    virtual bool canApproveOrders() const { return false; }

    const std::string& getPosition() const { return position_; }
    const Money& getSalary() const { return salary_; }
    const Date& getHireDate() const { return hireDate_; }
};
