#pragma once
#include <map>
#include <string>
#include <vector>
#include "Date.h"
#include "Employee.h"
#include "StockItem.h"
#include "StockMovement.h"
#include "WriteOffRecord.h"

class Inventory {
    std::map<std::string, StockItem> items_;
    std::vector<StockMovement> movements_;
    std::vector<WriteOffRecord> writeOffs_;
    int nextMovementId_ = 1;

public:
    StockItem& findItem(const ISBN& isbn) {
        auto it = items_.find(isbn.getCode());
        if (it == items_.end()) throw BookNotFoundException(isbn.getCode());
        return it->second;
    }

    void registerBook(const ISBN& isbn, int minLevel, const std::string& shelfId) {
        auto result = items_.emplace(isbn.getCode(), StockItem(isbn, 0, minLevel, shelfId));
        if (!result.second) throw DuplicateEntityException("позиция склада " + isbn.formatted());
    }

    bool hasBook(const ISBN& isbn) const { return items_.count(isbn.getCode()) > 0; }

    void receive(const ISBN& isbn, int quantity, const Employee& by, const Date& date) {
        findItem(isbn).receive(quantity);
        movements_.emplace_back(nextMovementId_++, MovementType::Receipt, isbn, quantity, date, by.getId());
    }

    void issue(const ISBN& isbn, int quantity, const Employee& by, const Date& date) {
        findItem(isbn).issue(quantity);
        movements_.emplace_back(nextMovementId_++, MovementType::Issue, isbn, quantity, date, by.getId());
    }

    void reserve(const ISBN& isbn, int quantity) { findItem(isbn).reserve(quantity); }
    void releaseReservation(const ISBN& isbn, int quantity) { findItem(isbn).releaseReservation(quantity); }
    int availableQuantity(const ISBN& isbn) { return findItem(isbn).available(); }

    int totalUnits() const {
        int sum = 0;
        for (const auto& entry : items_) sum += entry.second.getQuantity();
        return sum;
    }

    std::vector<std::string> lowStockItems() const {
        std::vector<std::string> result;
        for (const auto& entry : items_) {
            if (entry.second.needsReorder()) result.push_back(entry.first);
        }
        return result;
    }

    void writeOff(const ISBN& isbn, int quantity, const std::string& reason, const Employee& by, const Date& date) {
        findItem(isbn).issue(quantity);
        movements_.emplace_back(nextMovementId_++, MovementType::WriteOff, isbn, quantity, date, by.getId());
        writeOffs_.emplace_back(static_cast<int>(writeOffs_.size()) + 1, isbn, quantity, reason, date, by.getId());
    }

    std::vector<StockMovement> movementsFor(const ISBN& isbn) const {
        std::vector<StockMovement> result;
        for (const auto& m : movements_) {
            if (m.getIsbn().equals(isbn)) result.push_back(m);
        }
        return result;
    }

    int totalWrittenOff() const {
        int sum = 0;
        for (const auto& w : writeOffs_) sum += w.getQuantity();
        return sum;
    }

    const StockItem& getItem(const ISBN& isbn) const {
        auto it = items_.find(isbn.getCode());
        if (it == items_.end()) throw BookNotFoundException(isbn.getCode());
        return it->second;
    }
    const std::map<std::string, StockItem>& getItems() const { return items_; }
    const std::vector<StockMovement>& getMovements() const { return movements_; }
    const std::vector<WriteOffRecord>& getWriteOffs() const { return writeOffs_; }
};
