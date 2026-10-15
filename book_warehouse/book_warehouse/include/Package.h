#pragma once
#include <map>
#include <string>
#include <utility>
#include "Book.h"

class Package {
    int id_ = 0;
    int orderId_ = 0;
    std::map<std::string, int> items_;
    int weightGrams_ = 0;
    bool sealed_ = false;

public:
    Package(int id, int orderId) : id_(id), orderId_(orderId) {}

    void addItem(const Book& book, int quantity) {
        if (sealed_) throw InvalidOrderStateException("добавить в посылку", "посылка запечатана");
        if (quantity <= 0) throw InvalidDataException("количество должно быть положительным");
        items_[book.getIsbn().getCode()] += quantity;
        weightGrams_ += book.getWeightGrams() * quantity;
    }

    void seal() {
        if (items_.empty()) throw InvalidDataException("пустая посылка");
        sealed_ = true;
    }

    void unseal() { sealed_ = false; }
    bool isSealed() const { return sealed_; }
    double weightKg() const { return weightGrams_ / 1000.0; }

    int getId() const { return id_; }
    int getOrderId() const { return orderId_; }
    int getWeightGrams() const { return weightGrams_; }
};
