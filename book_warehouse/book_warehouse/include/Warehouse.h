#pragma once
#include <string>
#include <vector>
#include "Address.h"
#include "Inventory.h"
#include "LoadingDock.h"
#include "WarehouseManager.h"
#include "Zone.h"

class Warehouse {
    std::string id_;
    std::string name_;
    Address address_;
    std::vector<Zone> zones_;
    Inventory inventory_;
    std::vector<LoadingDock> docks_;
    std::string managerId_;

public:
    Warehouse() = default;
    Warehouse(std::string id, std::string name, Address address)
        : id_(std::move(id)), name_(std::move(name)), address_(std::move(address)) {}

    void addZone(const Zone& zone) {
        for (const auto& z : zones_) {
            if (z.getId() == zone.getId()) throw DuplicateEntityException("зона " + zone.getId());
        }
        zones_.push_back(zone);
    }

    void addDock(const LoadingDock& dock) { docks_.push_back(dock); }
    void assignManager(const WarehouseManager& manager) { managerId_ = manager.getId(); }

    Zone& findZone(const std::string& zoneId) {
        for (auto& z : zones_) {
            if (z.getId() == zoneId) return z;
        }
        throw InvalidDataException("зона не найдена: " + zoneId);
    }

    std::string locateBook(const ISBN& isbn) const {
        for (const auto& z : zones_) {
            std::string shelf = z.locate(isbn);
            if (!shelf.empty()) return z.getId() + "/" + shelf;
        }
        throw BookNotFoundException(isbn.getCode());
    }

    LoadingDock& acquireFreeDock(const std::string& carrier, const Date& date) {
        for (auto& d : docks_) {
            if (d.isFree()) {
                d.occupy(carrier, date);
                return d;
            }
        }
        throw InvalidOrderStateException("занять док", "свободных доков нет");
    }

    int freeDocksCount() const {
        int count = 0;
        for (const auto& d : docks_) {
            if (d.isFree()) ++count;
        }
        return count;
    }

    int totalShelves() const {
        int count = 0;
        for (const auto& z : zones_) count += static_cast<int>(z.getShelves().size());
        return count;
    }

    std::vector<std::string> zonesWithClimateViolation() const {
        std::vector<std::string> result;
        for (const auto& z : zones_) {
            if (!z.isClimateOk()) result.push_back(z.getId());
        }
        return result;
    }

    Inventory& getInventory() { return inventory_; }
    const Inventory& getInventory() const { return inventory_; }
    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const Address& getAddress() const { return address_; }
    const std::vector<Zone>& getZones() const { return zones_; }
    std::vector<LoadingDock>& getDocks() { return docks_; }
    const std::string& getManagerId() const { return managerId_; }
};
