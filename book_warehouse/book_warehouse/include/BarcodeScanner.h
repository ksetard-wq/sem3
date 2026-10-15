#pragma once
#include <string>
#include <utility>
#include "ISBN.h"

class BarcodeScanner {
    std::string id_;
    std::string lastScanned_;
    int batteryPercent_ = 100;
    int scanCount_ = 0;

public:
    BarcodeScanner() = default;
    explicit BarcodeScanner(std::string id) : id_(std::move(id)) {}

    ISBN scan(const std::string& rawCode) {
        if (batteryPercent_ <= 0) throw InvalidDataException("сканер " + id_ + " разряжен");
        ISBN result(rawCode);
        lastScanned_ = result.getCode();
        ++scanCount_;
        if (scanCount_ % 50 == 0) --batteryPercent_;
        return result;
    }

    bool verify(const ISBN& expected, const std::string& rawCode) { return scan(rawCode).equals(expected); }
    bool isBatteryLow() const { return batteryPercent_ < 15; }
    void recharge() { batteryPercent_ = 100; }
    void resetCounter() { scanCount_ = 0; }

    const std::string& getId() const { return id_; }
    const std::string& getLastScanned() const { return lastScanned_; }
    int getBatteryPercent() const { return batteryPercent_; }
    int getScanCount() const { return scanCount_; }
};
