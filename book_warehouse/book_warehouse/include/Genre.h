#pragma once
#include <string>
#include <utility>
#include "utils.h"

class Genre {
    std::string id_;
    std::string name_;
    int minAge_ = 0;

public:
    Genre() = default;
    Genre(std::string id, std::string name, int minAge)
        : id_(std::move(id)), name_(std::move(name)), minAge_(minAge) {}

    bool isSuitableForAge(int age) const { return age >= minAge_; }
    bool isChildrenGenre() const { return minAge_ < 12; }
    bool matchesName(const std::string& query) const { return containsIgnoreCase(name_, query); }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    int getMinAge() const { return minAge_; }
};
