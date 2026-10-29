#pragma once
#include <cmath>
#include "exceptions.h"

class GeoPoint {
    double latitude_ = 0.0;
    double longitude_ = 0.0;

public:
    GeoPoint() = default;
    GeoPoint(double latitude, double longitude) : latitude_(latitude), longitude_(longitude) {
        if (latitude < -90.0 || latitude > 90.0 || longitude < -180.0 || longitude > 180.0) {
            throw InvalidDataException("координаты вне допустимого диапазона");
        }
    }

    // Расстояние по большой окружности (формула гаверсинусов).
    double distanceKm(const GeoPoint& other) const {
        const double kPi = 3.14159265358979323846;
        auto rad = [kPi](double deg) { return deg * kPi / 180.0; };
        double dLat = rad(other.latitude_ - latitude_);
        double dLon = rad(other.longitude_ - longitude_);
        double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
                   std::cos(rad(latitude_)) * std::cos(rad(other.latitude_)) * std::sin(dLon / 2) * std::sin(dLon / 2);
        return 2.0 * 6371.0 * std::asin(std::sqrt(a));
    }

    bool isNear(const GeoPoint& other, double km) const { return distanceKm(other) <= km; }
    bool isNorthernHemisphere() const { return latitude_ >= 0.0; }
    bool isEasternHemisphere() const { return longitude_ >= 0.0; }

    double getLatitude() const { return latitude_; }
    double getLongitude() const { return longitude_; }
};
