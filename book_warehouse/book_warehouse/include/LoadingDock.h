#pragma once
#include <string>
#include <utility>
#include "Date.h"

class LoadingDock {
    std::string id_;
    bool occupied_ = false;
    std::string currentCarrier_;
    Date occupiedSince_;

public:
    LoadingDock() = default;
    explicit LoadingDock(std::string id) : id_(std::move(id)) {}

    void occupy(const std::string& carrier, const Date& date) {
        if (occupied_) throw InvalidOrderStateException("занять док " + id_, "док занят");
        occupied_ = true;
        currentCarrier_ = carrier;
        occupiedSince_ = date;
    }

    void release() {
        occupied_ = false;
        currentCarrier_.clear();
    }

    bool isFree() const { return !occupied_; }
    bool isOccupiedBy(const std::string& carrier) const { return occupied_ && currentCarrier_ == carrier; }

    const std::string& getId() const { return id_; }
    const std::string& getCurrentCarrier() const { return currentCarrier_; }
    const Date& getOccupiedSince() const { return occupiedSince_; }
};
