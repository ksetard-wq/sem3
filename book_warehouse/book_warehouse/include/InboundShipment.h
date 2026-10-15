#pragma once
#include <map>
#include <string>
#include <vector>
#include "LoadingDock.h"
#include "PurchaseOrder.h"
#include "Storekeeper.h"

class InboundShipment {
    int id_ = 0;
    PurchaseOrder purchaseOrder_;
    Date arrivalDate_;
    std::map<std::string, int> received_;
    std::string dockId_;
    bool inspected_ = false;
    std::string inspectedBy_;

public:
    InboundShipment(int id, PurchaseOrder purchaseOrder, Date arrivalDate)
        : id_(id), purchaseOrder_(std::move(purchaseOrder)), arrivalDate_(arrivalDate) {}

    void registerReceived(const ISBN& isbn, int quantity) {
        bool known = false;
        for (const auto& l : purchaseOrder_.getLines()) {
            if (l.getIsbn().equals(isbn)) known = true;
        }
        if (!known) throw BookNotFoundException(isbn.getCode() + " нет в заказе");
        if (quantity <= 0) throw InvalidDataException("количество должно быть положительным");
        received_[isbn.getCode()] += quantity;
    }

    bool isComplete() const { return missingLines().empty(); }

    std::vector<OrderLine> missingLines() const {
        std::vector<OrderLine> result;
        for (const auto& l : purchaseOrder_.getLines()) {
            auto it = received_.find(l.getIsbn().getCode());
            int got = it == received_.end() ? 0 : it->second;
            if (got < l.getQuantity()) result.emplace_back(l.getIsbn(), l.getQuantity() - got, l.getUnitPrice());
        }
        return result;
    }

    void inspect(const Storekeeper& keeper) {
        inspected_ = true;
        inspectedBy_ = keeper.getId();
    }

    bool isDelayed() const { return purchaseOrder_.getExpectedAt().isBefore(arrivalDate_); }

    void assignDock(const LoadingDock& dock) {
        if (!dock.isOccupiedBy(purchaseOrder_.getSupplier().getName())) {
            throw InvalidOrderStateException("назначить док " + dock.getId(), "док не занят этим поставщиком");
        }
        dockId_ = dock.getId();
    }

    int totalReceivedUnits() const {
        int sum = 0;
        for (const auto& entry : received_) sum += entry.second;
        return sum;
    }

    int getId() const { return id_; }
    const PurchaseOrder& getPurchaseOrder() const { return purchaseOrder_; }
    const Date& getArrivalDate() const { return arrivalDate_; }
    const std::string& getDockId() const { return dockId_; }
    bool getInspected() const { return inspected_; }
    const std::string& getInspectedBy() const { return inspectedBy_; }
};
