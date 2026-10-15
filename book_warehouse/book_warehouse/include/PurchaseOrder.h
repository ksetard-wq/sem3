#pragma once
#include <string>
#include <vector>
#include "Date.h"
#include "OrderLine.h"
#include "Supplier.h"

enum class PurchaseStatus { Draft, Approved, Sent, Received, Cancelled };

inline std::string toString(PurchaseStatus status) {
    switch (status) {
        case PurchaseStatus::Draft: return "Черновик";
        case PurchaseStatus::Approved: return "Утверждён";
        case PurchaseStatus::Sent: return "Отправлен";
        case PurchaseStatus::Received: return "Получен";
        default: return "Отменён";
    }
}

class PurchaseOrder {
    int id_ = 0;
    Supplier supplier_;
    std::vector<OrderLine> lines_;
    PurchaseStatus status_ = PurchaseStatus::Draft;
    Date createdAt_;
    Date expectedAt_;

public:
    PurchaseOrder(int id, Supplier supplier, Date createdAt, Date expectedAt)
        : id_(id), supplier_(std::move(supplier)), createdAt_(createdAt), expectedAt_(expectedAt) {}

    void addLine(const OrderLine& line) {
        if (status_ != PurchaseStatus::Draft) throw InvalidOrderStateException("добавить позицию", toString(status_));
        if (!supplier_.supplies(line.getIsbn())) throw BookNotFoundException(line.getIsbn().getCode() + " у поставщика");
        lines_.push_back(line);
    }

    Money totalCost() const {
        Money sum;
        for (const auto& l : lines_) sum = sum.add(l.total());
        return sum;
    }

    void approve() {
        if (status_ != PurchaseStatus::Draft || lines_.empty()) {
            throw InvalidOrderStateException("утвердить закупку", toString(status_));
        }
        status_ = PurchaseStatus::Approved;
    }

    void markSent() {
        if (status_ != PurchaseStatus::Approved) throw InvalidOrderStateException("отправить закупку", toString(status_));
        status_ = PurchaseStatus::Sent;
    }

    void markReceived() {
        if (status_ != PurchaseStatus::Sent) throw InvalidOrderStateException("принять закупку", toString(status_));
        status_ = PurchaseStatus::Received;
    }

    void cancel() {
        if (status_ == PurchaseStatus::Received) throw InvalidOrderStateException("отменить закупку", toString(status_));
        status_ = PurchaseStatus::Cancelled;
    }

    bool isOverdue(const Date& today) const { return status_ == PurchaseStatus::Sent && expectedAt_.isBefore(today); }

    int getId() const { return id_; }
    const Supplier& getSupplier() const { return supplier_; }
    const std::vector<OrderLine>& getLines() const { return lines_; }
    PurchaseStatus getStatus() const { return status_; }
    const Date& getCreatedAt() const { return createdAt_; }
    const Date& getExpectedAt() const { return expectedAt_; }
};
