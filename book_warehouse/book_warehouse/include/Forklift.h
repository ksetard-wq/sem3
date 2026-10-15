#pragma once
#include <string>
#include <utility>
#include "Loader.h"
#include "Pallet.h"

class Forklift {
    std::string id_;
    int capacityKg_ = 0;
    std::string operatorId_;
    int batteryPercent_ = 100;
    bool inUse_ = false;

public:
    Forklift(std::string id, int capacityKg) : id_(std::move(id)), capacityKg_(capacityKg) {}

    void assignOperator(const Loader& loader) {
        if (!loader.isForkliftCertified()) throw AccessDeniedException(loader.fullName(), "управлять погрузчиком");
        operatorId_ = loader.getId();
        inUse_ = true;
    }

    bool canLift(const Pallet& pallet) const { return pallet.getWeightGrams() <= capacityKg_ * 1000; }

    void liftPallet(const Pallet& pallet) {
        if (!inUse_) throw InvalidOrderStateException("поднять паллету", "нет оператора");
        if (!canLift(pallet)) throw ShelfOverloadException(id_);
        if (batteryPercent_ < 5) throw InvalidDataException("погрузчик " + id_ + " разряжен");
        batteryPercent_ -= 5;
    }

    void recharge() { batteryPercent_ = 100; }

    void releaseOperator() {
        operatorId_.clear();
        inUse_ = false;
    }

    const std::string& getId() const { return id_; }
    int getCapacityKg() const { return capacityKg_; }
    const std::string& getOperatorId() const { return operatorId_; }
    int getBatteryPercent() const { return batteryPercent_; }
    bool getInUse() const { return inUse_; }
};
