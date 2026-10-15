#pragma once
#include <string>
#include <vector>
#include "AuditDiscrepancy.h"
#include "Date.h"

class InventoryAudit {
    int id_ = 0;
    Date date_;
    std::string auditorId_;
    std::vector<AuditDiscrepancy> discrepancies_;
    bool closed_ = false;

public:
    InventoryAudit(int id, Date date, std::string auditorId) : id_(id), date_(date), auditorId_(std::move(auditorId)) {}

    void recordCount(const ISBN& isbn, int expected, int actual) {
        if (closed_) throw InvalidOrderStateException("запись подсчёта", "аудит закрыт");
        if (expected != actual) discrepancies_.emplace_back(isbn, expected, actual);
    }

    bool hasDiscrepancies() const { return !discrepancies_.empty(); }

    int totalShortage() const {
        int sum = 0;
        for (const auto& d : discrepancies_) {
            if (d.isShortage()) sum -= d.difference();
        }
        return sum;
    }

    int totalSurplus() const {
        int sum = 0;
        for (const auto& d : discrepancies_) {
            if (d.isSurplus()) sum += d.difference();
        }
        return sum;
    }

    void close() { closed_ = true; }
    bool isClosed() const { return closed_; }

    int getId() const { return id_; }
    const Date& getDate() const { return date_; }
    const std::string& getAuditorId() const { return auditorId_; }
    const std::vector<AuditDiscrepancy>& getDiscrepancies() const { return discrepancies_; }
};
