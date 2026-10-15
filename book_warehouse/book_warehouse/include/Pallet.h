#pragma once
#include <map>
#include <string>
#include <utility>
#include "Book.h"

class Pallet {
    std::string id_;
    std::map<std::string, int> items_;
    int weightGrams_ = 0;
    int maxWeightGrams_ = 0;

public:
    Pallet() = default;
    Pallet(std::string id, int maxWeightGrams) : id_(std::move(id)), maxWeightGrams_(maxWeightGrams) {}

    bool canLoad(const Book& book, int quantity) const {
        return weightGrams_ + book.getWeightGrams() * quantity <= maxWeightGrams_;
    }

    void load(const Book& book, int quantity) {
        if (quantity <= 0) throw InvalidDataException("количество должно быть положительным");
        if (!canLoad(book, quantity)) throw ShelfOverloadException(id_);
        items_[book.getIsbn().getCode()] += quantity;
        weightGrams_ += book.getWeightGrams() * quantity;
    }

    void unloadAll() {
        items_.clear();
        weightGrams_ = 0;
    }

    int bookCount() const {
        int sum = 0;
        for (const auto& entry : items_) sum += entry.second;
        return sum;
    }

    const std::string& getId() const { return id_; }
    int getWeightGrams() const { return weightGrams_; }
    int getMaxWeightGrams() const { return maxWeightGrams_; }
};
