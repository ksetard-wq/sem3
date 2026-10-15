#pragma once
#include <map>
#include <string>
#include <utility>
#include "Address.h"
#include "ContactInfo.h"
#include "ISBN.h"
#include "Money.h"

class Supplier {
    std::string id_;
    std::string name_;
    Address address_;
    ContactInfo contact_;
    std::map<std::string, Money> priceList_;
    double rating_ = 3.0;
    bool active_ = true;

public:
    Supplier() = default;
    Supplier(std::string id, std::string name, Address address, ContactInfo contact)
        : id_(std::move(id)), name_(std::move(name)), address_(std::move(address)), contact_(std::move(contact)) {}

    void addPrice(const ISBN& isbn, const Money& price) {
        if (price.isNegative() || price.isZero()) throw InvalidDataException("закупочная цена должна быть положительной");
        priceList_[isbn.getCode()] = price;
    }

    Money priceFor(const ISBN& isbn) const {
        auto it = priceList_.find(isbn.getCode());
        if (it == priceList_.end()) throw BookNotFoundException(isbn.getCode() + " у поставщика " + name_);
        return it->second;
    }

    bool supplies(const ISBN& isbn) const { return active_ && priceList_.count(isbn.getCode()) > 0; }

    void rate(double rating) {
        if (rating < 0.0 || rating > 5.0) throw InvalidDataException("рейтинг от 0 до 5");
        rating_ = rating;
    }

    void deactivate() { active_ = false; }
    void activate() { active_ = true; }
    bool isReliable() const { return active_ && rating_ >= 4.0; }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const Address& getAddress() const { return address_; }
    const ContactInfo& getContact() const { return contact_; }
    double getRating() const { return rating_; }
    bool getActive() const { return active_; }
};
