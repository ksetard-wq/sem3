#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Book.h"
#include "ClimateControl.h"
#include "Shelf.h"

class Zone {
    std::string id_;
    std::string name_;
    std::vector<Shelf> shelves_;
    ClimateControl climate_;

public:
    Zone() = default;
    Zone(std::string id, std::string name) : id_(std::move(id)), name_(std::move(name)) {}

    void addShelf(const Shelf& shelf) {
        for (const auto& s : shelves_) {
            if (s.getId() == shelf.getId()) throw DuplicateEntityException("стеллаж " + shelf.getId());
        }
        shelves_.push_back(shelf);
    }

    Shelf* findShelfFor(const Book& book, int quantity) {
        for (auto& s : shelves_) {
            if (s.canHold(book, quantity)) return &s;
        }
        return nullptr;
    }

    std::string placeBook(const Book& book, int quantity) {
        Shelf* shelf = findShelfFor(book, quantity);
        if (shelf == nullptr) throw ShelfOverloadException(id_);
        shelf->place(book, quantity);
        return shelf->getId();
    }

    int quantityOf(const ISBN& isbn) const {
        int sum = 0;
        for (const auto& s : shelves_) sum += s.quantityOf(isbn);
        return sum;
    }

    void removeBook(const ISBN& isbn, int quantity) {
        int total = quantityOf(isbn);
        if (quantity <= 0 || total < quantity) throw InsufficientStockException(isbn.getCode(), quantity, total);
        int left = quantity;
        for (auto& s : shelves_) {
            int take = std::min(s.quantityOf(isbn), left);
            if (take > 0) {
                s.take(isbn, take);
                left -= take;
            }
        }
    }

    std::string locate(const ISBN& isbn) const {
        for (const auto& s : shelves_) {
            if (s.quantityOf(isbn) > 0) return s.getId();
        }
        return "";
    }

    long long totalCapacityGrams() const {
        long long sum = 0;
        for (const auto& s : shelves_) sum += s.getMaxWeightGrams();
        return sum;
    }

    bool isClimateOk() const { return climate_.isWithinRange(); }
    void updateClimate(double temperature, double humidity) { climate_.updateReadings(temperature, humidity); }
    void assertClimate() const { climate_.assertWithinRange(id_); }

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::vector<Shelf>& getShelves() const { return shelves_; }
    const ClimateControl& getClimate() const { return climate_; }
};
