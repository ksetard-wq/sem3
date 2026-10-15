#pragma once
#include <string>
#include <utility>
#include "ISBN.h"
#include "exceptions.h"

class StockItem {
    ISBN isbn_;
    int quantity_ = 0;
    int reserved_ = 0;
    int minLevel_ = 0;
    std::string shelfId_;

public:
    StockItem(ISBN isbn, int quantity, int minLevel, std::string shelfId)
        : isbn_(std::move(isbn)), quantity_(quantity), minLevel_(minLevel), shelfId_(std::move(shelfId)) {}

    static void requirePositive(int amount) {
        if (amount <= 0) throw InvalidDataException("количество должно быть положительным");
    }

    int available() const { return quantity_ - reserved_; }

    void receive(int amount) {
        requirePositive(amount);
        quantity_ += amount;
    }

    void issue(int amount) {
        requirePositive(amount);
        if (amount > available()) throw InsufficientStockException(isbn_.getCode(), amount, available());
        quantity_ -= amount;
    }

    void reserve(int amount) {
        requirePositive(amount);
        if (amount > available()) throw InsufficientStockException(isbn_.getCode(), amount, available());
        reserved_ += amount;
    }

    void releaseReservation(int amount) {
        requirePositive(amount);
        if (amount > reserved_) throw InvalidDataException("снимается больше резерва, чем существует");
        reserved_ -= amount;
    }

    bool needsReorder() const { return available() <= minLevel_; }
    void moveToShelf(const std::string& shelfId) { shelfId_ = shelfId; }

    const ISBN& getIsbn() const { return isbn_; }
    int getQuantity() const { return quantity_; }
    int getReserved() const { return reserved_; }
    int getMinLevel() const { return minLevel_; }
    const std::string& getShelfId() const { return shelfId_; }
};
