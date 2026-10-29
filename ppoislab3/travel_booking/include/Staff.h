#pragma once
#include <string>
#include <utility>
#include "Money.h"
#include "Person.h"

class Staff : public Person {
    std::string department_;
    Date hireDate_;
    Money salary_;

public:
    Staff() = default;
    Staff(std::string id, std::string firstName, std::string lastName, ContactInfo contact, Date birthDate,
          std::string department, Money salary, Date hireDate)
        : Person(std::move(id), std::move(firstName), std::move(lastName), std::move(contact), birthDate),
          department_(std::move(department)), hireDate_(hireDate), salary_(std::move(salary)) {}

    int yearsOfService(const Date& today) const { return hireDate_.daysUntil(today) / 365; }

    void raiseSalary(double percent) {
        if (percent <= 0) throw InvalidDataException("процент повышения должен быть положительным");
        salary_ = salary_.add(salary_.percentOf(percent));
    }

    virtual std::string role() const { return "Сотрудник"; }
    virtual bool canRefund() const { return false; }

    const std::string& getDepartment() const { return department_; }
    const Date& getHireDate() const { return hireDate_; }
    const Money& getSalary() const { return salary_; }
};
