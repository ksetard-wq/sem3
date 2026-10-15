#pragma once
#include <map>
#include <string>
#include <utility>
#include "Book.h"

class Shelf {
    std::string id_;
    int maxWeightGrams_ = 0;
    int currentWeightGrams_ = 0;
    std::map<std::string, int> contents_;
    std::map<std::string, int> unitWeights_;

public:
    Shelf() = default;
    Shelf(std::string id, int maxWeightGrams) : id_(std::move(id)), maxWeightGrams_(maxWeightGrams) {}

    int freeWeightGrams() const { return maxWeightGrams_ - currentWeightGrams_; }
    bool canHold(const Book& book, int quantity) const { return book.getWeightGrams() * quantity <= freeWeightGrams(); }

    void place(const Book& book, int quantity) {
        if (quantity <= 0) throw InvalidDataException("количество должно быть положительным");
        if (!canHold(book, quantity)) throw ShelfOverloadException(id_);
        contents_[book.getIsbn().getCode()] += quantity;
        unitWeights_[book.getIsbn().getCode()] = book.getWeightGrams();
        currentWeightGrams_ += book.getWeightGrams() * quantity;
    }

    void take(const ISBN& isbn, int quantity) {
        int have = quantityOf(isbn);
        if (quantity <= 0 || have < quantity) throw InsufficientStockException(isbn.getCode(), quantity, have);
        contents_[isbn.getCode()] -= quantity;
        currentWeightGrams_ -= unitWeights_[isbn.getCode()] * quantity;
    }

    int quantityOf(const ISBN& isbn) const {
        auto it = contents_.find(isbn.getCode());
        return it == contents_.end() ? 0 : it->second;
    }

    bool isEmpty() const { return currentWeightGrams_ == 0; }
    double loadPercent() const { return maxWeightGrams_ == 0 ? 0.0 : 100.0 * currentWeightGrams_ / maxWeightGrams_; }

    const std::string& getId() const { return id_; }
    int getMaxWeightGrams() const { return maxWeightGrams_; }
    int getCurrentWeightGrams() const { return currentWeightGrams_; }
};
