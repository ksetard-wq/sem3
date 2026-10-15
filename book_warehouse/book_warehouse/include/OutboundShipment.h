#pragma once
#include <string>
#include "Address.h"
#include "Package.h"
#include "ShippingCarrier.h"

enum class ShipmentStatus { Created, InTransit, Delivered, Returned };

inline std::string toString(ShipmentStatus status) {
    switch (status) {
        case ShipmentStatus::Created: return "Создана";
        case ShipmentStatus::InTransit: return "В пути";
        case ShipmentStatus::Delivered: return "Доставлена";
        default: return "Возвращена";
    }
}

class OutboundShipment {
    int id_ = 0;
    Package package_;
    ShippingCarrier carrier_;
    std::string trackingNumber_;
    ShipmentStatus status_ = ShipmentStatus::Created;
    Date shippedAt_;
    Address destination_;

public:
    OutboundShipment(int id, Package package, ShippingCarrier carrier, Address destination)
        : id_(id), package_(std::move(package)), carrier_(std::move(carrier)), destination_(std::move(destination)) {}

    std::string generateTrackingNumber() const {
        return "TRK-" + std::to_string(id_) + "-" + std::to_string(package_.getId());
    }

    void dispatch(const Date& date) {
        if (status_ != ShipmentStatus::Created) throw InvalidOrderStateException("отправить", toString(status_));
        if (!package_.isSealed()) throw InvalidOrderStateException("отправить", "посылка не запечатана");
        if (!carrier_.canCarry(package_)) throw ShelfOverloadException("перевозчик " + carrier_.getName());
        trackingNumber_ = generateTrackingNumber();
        shippedAt_ = date;
        status_ = ShipmentStatus::InTransit;
    }

    void markDelivered() {
        if (status_ != ShipmentStatus::InTransit) throw InvalidOrderStateException("доставить", toString(status_));
        status_ = ShipmentStatus::Delivered;
    }

    void markReturned() {
        if (status_ != ShipmentStatus::InTransit) throw InvalidOrderStateException("вернуть", toString(status_));
        status_ = ShipmentStatus::Returned;
    }

    bool isDelivered() const { return status_ == ShipmentStatus::Delivered; }
    Money cost() const { return carrier_.calculateCost(package_); }
    Date eta() const { return carrier_.estimateDelivery(shippedAt_); }

    int getId() const { return id_; }
    const Package& getPackage() const { return package_; }
    const ShippingCarrier& getCarrier() const { return carrier_; }
    const std::string& getTrackingNumber() const { return trackingNumber_; }
    ShipmentStatus getStatus() const { return status_; }
    const Date& getShippedAt() const { return shippedAt_; }
    const Address& getDestination() const { return destination_; }
};
