#pragma once
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "BookCatalog.h"
#include "CustomerOrder.h"
#include "Inventory.h"
#include "PayrollRecord.h"
#include "Supplier.h"

class ReportGenerator {
    std::string title_;
    Date generatedAt_;

public:
    ReportGenerator(std::string title, Date generatedAt) : title_(std::move(title)), generatedAt_(generatedAt) {}

    std::string header() const { return "=== " + title_ + " (" + generatedAt_.toString() + ") ===\n"; }

    std::string stockReport(const Inventory& inventory, const BookCatalog& catalog) const {
        std::ostringstream out;
        out << header();
        for (const auto& entry : inventory.getItems()) {
            const StockItem& item = entry.second;
            out << catalog.findByIsbn(item.getIsbn()).getTitle() << " | на складе: " << item.getQuantity()
                << " | резерв: " << item.getReserved() << " | доступно: " << item.available() << "\n";
        }
        out << "Всего единиц: " << inventory.totalUnits() << "\n";
        return out.str();
    }

    std::string lowStockReport(const Inventory& inventory, const BookCatalog& catalog) const {
        std::ostringstream out;
        out << header();
        std::vector<std::string> low = inventory.lowStockItems();
        if (low.empty()) out << "Все позиции в достаточном количестве\n";
        for (const auto& code : low) {
            out << "Требуется пополнение: " << catalog.findByIsbn(ISBN(code)).getTitle() << "\n";
        }
        return out.str();
    }

    std::string salesReport(const std::vector<CustomerOrder>& orders) const {
        std::ostringstream out;
        out << header();
        Money sum;
        for (const auto& o : orders) {
            out << "Заказ №" << o.getId() << " | " << o.getCustomer().fullName() << " | " << toString(o.getStatus())
                << " | " << o.total().toString() << "\n";
            if (o.getStatus() != OrderStatus::Cancelled) sum = sum.add(o.total());
        }
        out << "Итого по неотменённым заказам: " << sum.toString() << "\n";
        return out.str();
    }

    std::string payrollReport(const std::vector<PayrollRecord>& records) const {
        std::ostringstream out;
        out << header();
        for (const auto& r : records) {
            out << r.getEmployeeId() << " | " << r.getPeriod() << " | к выплате: " << r.netPay().toString() << "\n";
        }
        return out.str();
    }

    std::string supplierReport(const std::vector<Supplier>& suppliers) const {
        std::ostringstream out;
        out << header();
        for (const auto& s : suppliers) {
            out << s.getName() << " | рейтинг: " << s.getRating() << (s.isReliable() ? " | надёжный" : " | требует внимания") << "\n";
        }
        return out.str();
    }

    const std::string& getTitle() const { return title_; }
    const Date& getGeneratedAt() const { return generatedAt_; }
};
