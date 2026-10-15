#pragma once
#include <string>
#include "Employee.h"
#include "Payment.h"
#include "PayrollRecord.h"
#include "SupplierInvoice.h"
#include "TaxCalculator.h"

class Accountant : public Employee {
    Money ledgerBalance_;
    int processedInvoices_ = 0;

public:
    Accountant(const Employee& base, Money openingBalance) : Employee(base), ledgerBalance_(std::move(openingBalance)) {}

    void payInvoice(SupplierInvoice& invoice, const Payment& payment) {
        invoice.settle(payment);
        ledgerBalance_ = ledgerBalance_.subtract(payment.getAmount());
        ++processedInvoices_;
    }

    void recordIncome(const Money& amount) { ledgerBalance_ = ledgerBalance_.add(amount); }
    void recordExpense(const Money& amount) { ledgerBalance_ = ledgerBalance_.subtract(amount); }

    PayrollRecord buildPayroll(const Employee& employee, const std::string& period, const Money& bonus,
                               const TaxCalculator& tax) const {
        Money gross = employee.getSalary().add(bonus);
        return PayrollRecord(employee.getId(), period, employee.getSalary(), bonus, tax.incomeTax(gross));
    }

    bool isInDeficit() const { return ledgerBalance_.isNegative(); }

    std::string role() const override { return "Бухгалтер"; }

    const Money& getLedgerBalance() const { return ledgerBalance_; }
    int getProcessedInvoices() const { return processedInvoices_; }
};
