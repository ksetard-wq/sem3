#pragma once
#include <string>
#include "exceptions.h"

class ClimateControl {
    double minTemp_ = 15.0;
    double maxTemp_ = 25.0;
    double minHumidity_ = 30.0;
    double maxHumidity_ = 60.0;
    double currentTemp_ = 20.0;
    double currentHumidity_ = 45.0;

public:
    ClimateControl() = default;

    void updateReadings(double temperature, double humidity) {
        currentTemp_ = temperature;
        currentHumidity_ = humidity;
    }

    bool isTemperatureOk() const { return currentTemp_ >= minTemp_ && currentTemp_ <= maxTemp_; }
    bool isHumidityOk() const { return currentHumidity_ >= minHumidity_ && currentHumidity_ <= maxHumidity_; }
    bool isWithinRange() const { return isTemperatureOk() && isHumidityOk(); }

    void assertWithinRange(const std::string& zoneId) const {
        if (!isWithinRange()) throw ClimateViolationException(zoneId, currentTemp_, currentHumidity_);
    }

    void adjustLimits(double minTemp, double maxTemp) {
        if (minTemp >= maxTemp) throw InvalidDataException("минимальная температура должна быть меньше максимальной");
        minTemp_ = minTemp;
        maxTemp_ = maxTemp;
    }

    double getCurrentTemp() const { return currentTemp_; }
    double getCurrentHumidity() const { return currentHumidity_; }
};
