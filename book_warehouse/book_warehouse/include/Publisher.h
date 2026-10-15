#pragma once
#include <string>
#include <utility>
#include "Address.h"
#include "ISBN.h"

class Publisher {
    std::string id_;
    std::string name_;
    Address address_;
    std::string isbnPrefix_;
    int foundedYear_ = 0;

public:
    Publisher() = default;
    Publisher(std::string id, std::string name, Address address, std::string isbnPrefix, int foundedYear)
        : id_(std::move(id)), name_(std::move(name)), address_(std::move(address)),
          isbnPrefix_(std::move(isbnPrefix)), foundedYear_(foundedYear) {}

    bool ownsIsbn(const ISBN& isbn) const { return isbn.hasPrefix(isbnPrefix_); }
    int yearsActive(int currentYear) const { return currentYear - foundedYear_; }
    void relocate(const Address& newAddress) { address_ = newAddress; }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const Address& getAddress() const { return address_; }
    const std::string& getIsbnPrefix() const { return isbnPrefix_; }
};
