#pragma once
#include <string>
#include "BarcodeScanner.h"
#include "Book.h"
#include "Employee.h"
#include "InventoryAudit.h"
#include "Zone.h"

class Storekeeper : public Employee {
    std::string assignedZoneId_;
    BarcodeScanner scanner_;

public:
    Storekeeper(const Employee& base, std::string assignedZoneId, BarcodeScanner scanner)
        : Employee(base), assignedZoneId_(std::move(assignedZoneId)), scanner_(std::move(scanner)) {}

    void requireZone(const Zone& zone) const {
        if (zone.getId() != assignedZoneId_) throw AccessDeniedException(fullName(), "работа с зоной " + zone.getId());
    }

    std::string placeBook(Zone& zone, const Book& book, int quantity) const {
        requireZone(zone);
        return zone.placeBook(book, quantity);
    }

    void takeBook(Zone& zone, const ISBN& isbn, int quantity) const {
        requireZone(zone);
        zone.removeBook(isbn, quantity);
    }

    void countBook(const Zone& zone, const ISBN& isbn, int expected, InventoryAudit& audit) const {
        requireZone(zone);
        audit.recordCount(isbn, expected, zone.quantityOf(isbn));
    }

    void equipScanner(const BarcodeScanner& scanner) { scanner_ = scanner; }
    bool verifyBook(const ISBN& expected, const std::string& rawCode) { return scanner_.verify(expected, rawCode); }

    std::string role() const override { return "Кладовщик"; }

    const std::string& getAssignedZoneId() const { return assignedZoneId_; }
    const BarcodeScanner& getScanner() const { return scanner_; }
};
