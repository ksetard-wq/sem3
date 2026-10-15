#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "ISBN.h"

class Author {
    std::string id_;
    std::string name_;
    std::string country_;
    int birthYear_ = 0;
    std::vector<std::string> works_;

public:
    Author() = default;
    Author(std::string id, std::string name, std::string country, int birthYear)
        : id_(std::move(id)), name_(std::move(name)), country_(std::move(country)), birthYear_(birthYear) {}

    int ageIn(int year) const { return year - birthYear_; }
    bool isContemporary(int currentYear) const { return birthYear_ > currentYear - 100; }

    void addWork(const ISBN& isbn) {
        if (!hasWritten(isbn)) works_.push_back(isbn.getCode());
    }

    bool hasWritten(const ISBN& isbn) const {
        return std::find(works_.begin(), works_.end(), isbn.getCode()) != works_.end();
    }

    size_t worksCount() const { return works_.size(); }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getCountry() const { return country_; }
    int getBirthYear() const { return birthYear_; }
};
