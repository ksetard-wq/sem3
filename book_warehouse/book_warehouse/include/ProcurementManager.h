#pragma once
#include <vector>
#include "Employee.h"
#include "PurchaseOrder.h"

class ProcurementManager : public Employee {
    Money budget_;
    int approvedCount_ = 0;

public:
    ProcurementManager(const Employee& base, Money budget) : Employee(base), budget_(std::move(budget)) {}

    PurchaseOrder createPurchaseOrder(int id, const Supplier& supplier, const ISBN& isbn, int quantity,
                                      const Date& createdAt, const Date& expectedAt) const {
        PurchaseOrder order(id, supplier, createdAt, expectedAt);
        order.addLine(OrderLine(isbn, quantity, supplier.priceFor(isbn)));
        return order;
    }

    void approveOrder(PurchaseOrder& order) {
        Money cost = order.totalCost();
        if (cost.isGreaterThan(budget_)) throw AccessDeniedException(fullName(), "утвердить закупку выше бюджета");
        order.approve();
        budget_ = budget_.subtract(cost);
        ++approvedCount_;
    }

    const Supplier& chooseCheapestSupplier(const std::vector<Supplier>& suppliers, const ISBN& isbn) const {
        const Supplier* best = nullptr;
        for (const auto& s : suppliers) {
            if (s.supplies(isbn) && (best == nullptr || best->priceFor(isbn).isGreaterThan(s.priceFor(isbn)))) {
                best = &s;
            }
        }
        if (best == nullptr) throw SupplierNotFoundException(isbn.getCode());
        return *best;
    }

    void replenishBudget(const Money& amount) { budget_ = budget_.add(amount); }

    std::string role() const override { return "Менеджер по закупкам"; }
    bool canApproveOrders() const override { return true; }

    const Money& getBudget() const { return budget_; }
    int getApprovedCount() const { return approvedCount_; }
};
