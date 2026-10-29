#pragma once
#include <string>
#include <vector>
#include "Passport.h"
#include "Person.h"
#include "Visa.h"

class Traveler : public Person {
    Passport passport_;
    Country citizenship_;
    std::vector<Visa> visas_;

public:
    Traveler(std::string id, std::string firstName, std::string lastName, ContactInfo contact, Date birthDate,
             Passport passport, Country citizenship)
        : Person(std::move(id), std::move(firstName), std::move(lastName), std::move(contact), birthDate),
          passport_(std::move(passport)), citizenship_(std::move(citizenship)) {}

    void addVisa(const Visa& visa) { visas_.push_back(visa); }
    void updatePassport(const Passport& passport) { passport_ = passport; }
    bool hasValidPassport(const DateRange& trip) const { return passport_.isFormatCorrect() && passport_.isValidForTrip(trip); }

    bool hasVisaFor(const Country& destination, const DateRange& trip) const {
        for (const auto& v : visas_) {
            if (v.getCountry().isSameCountry(destination) && v.coversTrip(trip)) return true;
        }
        return false;
    }

    bool canEnter(const Country& destination, const DateRange& trip) const {
        return hasValidPassport(trip) && (destination.isVisaFreeFor(citizenship_) || hasVisaFor(destination, trip));
    }

    void requireDocuments(const Country& destination, const DateRange& trip) const {
        if (!passport_.isFormatCorrect()) throw InvalidDocumentException("неверный формат номера паспорта");
        if (!passport_.isValidForTrip(trip)) {
            throw InvalidDocumentException("паспорт истекает " + passport_.getExpiry().toString() +
                                           " — менее чем через 6 месяцев после поездки");
        }
        if (!destination.isVisaFreeFor(citizenship_) && !hasVisaFor(destination, trip)) {
            throw VisaRequiredException(destination.getName());
        }
    }

    bool isMinor(const Date& today) const { return !isAdult(today); }

    const Passport& getPassport() const { return passport_; }
    const Country& getCitizenship() const { return citizenship_; }
    const std::vector<Visa>& getVisas() const { return visas_; }
};
