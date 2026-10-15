#pragma once
#include <string>
#include "Employee.h"
#include "InventoryAudit.h"
#include "Schedule.h"
#include "WriteOffRecord.h"

class WarehouseManager : public Employee {
    std::string warehouseId_;
    Money approvalLimit_;

public:
    WarehouseManager(const Employee& base, std::string warehouseId, Money approvalLimit)
        : Employee(base), warehouseId_(std::move(warehouseId)), approvalLimit_(std::move(approvalLimit)) {}

    bool canApprove(const Money& amount) const { return !amount.isGreaterThan(approvalLimit_); }

    bool approveWriteOff(const WriteOffRecord& record, const Money& unitCost) const {
        return canApprove(record.lossAmount(unitCost));
    }

    void assignShift(Schedule& schedule, const Shift& shift) const { schedule.addShift(shift); }

    void closeAudit(InventoryAudit& audit) const {
        if (audit.isClosed()) throw InvalidOrderStateException("закрытие аудита", "аудит уже закрыт");
        audit.close();
    }

    void raiseApprovalLimit(const Money& extra) { approvalLimit_ = approvalLimit_.add(extra); }

    std::string role() const override { return "Заведующий складом"; }
    bool canApproveOrders() const override { return true; }

    const std::string& getWarehouseId() const { return warehouseId_; }
    const Money& getApprovalLimit() const { return approvalLimit_; }
};
