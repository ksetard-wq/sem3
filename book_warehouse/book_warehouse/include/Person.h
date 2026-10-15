#pragma once
#include <string>
#include <utility>
#include "ContactInfo.h"
#include "Date.h"

class Person {
    std::string id_;
    std::string firstName_;
    std::string lastName_;
    ContactInfo contact_;
    Date birthDate_;

public:
    Person() = default;
    Person(std::string id, std::string firstName, std::string lastName, ContactInfo contact, Date birthDate)
        : id_(std::move(id)), firstName_(std::move(firstName)), lastName_(std::move(lastName)),
          contact_(std::move(contact)), birthDate_(birthDate) {}
    virtual ~Person() = default;

    std::string fullName() const { return lastName_ + " " + firstName_; }

    int ageOn(const Date& today) const {
        int age = today.getYear() - birthDate_.getYear();
        if (today.getMonth() < birthDate_.getMonth() ||
            (today.getMonth() == birthDate_.getMonth() && today.getDay() < birthDate_.getDay())) {
            --age;
        }
        return age;
    }

    bool isAdult(const Date& today) const { return ageOn(today) >= 18; }
    void updateContact(const ContactInfo& contact) { contact_ = contact; }

    const std::string& getId() const { return id_; }
    const std::string& getFirstName() const { return firstName_; }
    const std::string& getLastName() const { return lastName_; }
    const ContactInfo& getContact() const { return contact_; }
    const Date& getBirthDate() const { return birthDate_; }
};
