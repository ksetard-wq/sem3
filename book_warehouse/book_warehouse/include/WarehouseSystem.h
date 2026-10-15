#pragma once
#include <string>
#include <vector>
#include "ActivityLog.h"
#include "BookCatalog.h"
#include "Customer.h"
#include "CustomerOrder.h"
#include "Employee.h"
#include "PricingPolicy.h"
#include "PurchaseOrder.h"
#include "Supplier.h"
#include "Warehouse.h"

// Фасад всей системы: связывает каталог, склад, поставщиков, клиентов и заказы.
class WarehouseSystem {
    BookCatalog catalog_;
    Warehouse warehouse_;
    std::vector<Supplier> suppliers_;
    std::vector<Customer> customers_;
    std::vector<CustomerOrder> orders_;
    std::vector<PurchaseOrder> purchaseOrders_;
    ActivityLog log_;
    PricingPolicy pricing_;
    int nextOrderId_ = 1;
    int nextPurchaseId_ = 1;

public:
    WarehouseSystem(Warehouse warehouse, PricingPolicy pricing)
        : warehouse_(std::move(warehouse)), pricing_(std::move(pricing)) {}

    void addBook(const Book& book, int minLevel, const std::string& shelfId, const Date& today) {
        catalog_.addBook(book);
        warehouse_.getInventory().registerBook(book.getIsbn(), minLevel, shelfId);
        log_.log("system", "добавлена книга " + book.getTitle(), today);
    }

    void addSupplier(const Supplier& supplier) {
        for (const auto& s : suppliers_) {
            if (s.getId() == supplier.getId()) throw DuplicateEntityException("поставщик " + supplier.getId());
        }
        suppliers_.push_back(supplier);
    }

    void registerCustomer(const Customer& customer) {
        for (const auto& c : customers_) {
            if (c.getId() == customer.getId()) throw DuplicateEntityException("клиент " + customer.getId());
        }
        customers_.push_back(customer);
    }

    Customer& findCustomer(const std::string& id) {
        for (auto& c : customers_) {
            if (c.getId() == id) return c;
        }
        throw InvalidDataException("клиент не найден: " + id);
    }

    Supplier& findSupplier(const std::string& id) {
        for (auto& s : suppliers_) {
            if (s.getId() == id) return s;
        }
        throw SupplierNotFoundException(id);
    }

    CustomerOrder& findOrder(int id) {
        for (auto& o : orders_) {
            if (o.getId() == id) return o;
        }
        throw OrderNotFoundException(id);
    }

    PurchaseOrder& findPurchase(int id) {
        for (auto& p : purchaseOrders_) {
            if (p.getId() == id) return p;
        }
        throw OrderNotFoundException(id);
    }

    int createOrder(const std::string& customerId, const Date& today) {
        Customer& customer = findCustomer(customerId);
        if (!customer.canOrder()) throw AccessDeniedException(customer.fullName(), "оформить заказ");
        int id = nextOrderId_++;
        orders_.emplace_back(id, customer, today, customer.getAddress());
        customer.registerOrder(id);
        log_.log(customerId, "создан заказ №" + std::to_string(id), today);
        return id;
    }

    void addToOrder(int orderId, const ISBN& isbn, int quantity) {
        CustomerOrder& order = findOrder(orderId);
        const Book& book = catalog_.findByIsbn(isbn);
        if (!book.isSuitableForAge(order.getCustomer().ageOn(order.getCreatedAt()))) {
            throw AccessDeniedException(order.getCustomer().fullName(), "купить книгу с возрастным ограничением");
        }
        warehouse_.getInventory().reserve(isbn, quantity);
        try {
            order.addLine(OrderLine(isbn, quantity, book.getPrice()));
        } catch (...) {
            warehouse_.getInventory().releaseReservation(isbn, quantity);
            throw;
        }
    }

    void confirmOrder(int orderId, const Date& today) {
        findOrder(orderId).confirm();
        log_.log("system", "подтверждён заказ №" + std::to_string(orderId), today);
    }

    Payment payOrder(int orderId, PaymentMethod method, const Date& today) {
        CustomerOrder& order = findOrder(orderId);
        Payment payment(orderId, order.total(), method, today);
        payment.process();
        order.pay(payment);
        findCustomer(order.getCustomer().getId()).earnPoints(payment.getAmount());
        log_.log("cashier", "оплачен заказ №" + std::to_string(orderId), today);
        return payment;
    }

    void fulfillOrder(int orderId, const Employee& by, const Date& today) {
        CustomerOrder& order = findOrder(orderId);
        order.pack();
        for (const auto& line : order.getLines()) {
            warehouse_.getInventory().releaseReservation(line.getIsbn(), line.getQuantity());
            warehouse_.getInventory().issue(line.getIsbn(), line.getQuantity(), by, today);
        }
        log_.log(by.getId(), "укомплектован заказ №" + std::to_string(orderId), today);
    }

    void shipOrder(int orderId, const Date& today) {
        findOrder(orderId).ship();
        log_.log("logistics", "отгружен заказ №" + std::to_string(orderId), today);
    }

    void cancelOrder(int orderId, const Date& today) {
        CustomerOrder& order = findOrder(orderId);
        order.cancel();
        for (const auto& line : order.getLines()) {
            warehouse_.getInventory().releaseReservation(line.getIsbn(), line.getQuantity());
        }
        log_.log("system", "отменён заказ №" + std::to_string(orderId), today);
    }

    Money revenue() const {
        Money sum;
        for (const auto& o : orders_) {
            if (o.isPaid() && o.getStatus() != OrderStatus::Cancelled) sum = sum.add(o.total());
        }
        return sum;
    }

    int createPurchase(const std::string& supplierId, const ISBN& isbn, int quantity, const Date& today) {
        Supplier& supplier = findSupplier(supplierId);
        int id = nextPurchaseId_++;
        PurchaseOrder order(id, supplier, today, today.addDays(7));
        order.addLine(OrderLine(isbn, quantity, supplier.priceFor(isbn)));
        purchaseOrders_.push_back(order);
        log_.log("procurement", "создана закупка №" + std::to_string(id), today);
        return id;
    }

    void receivePurchase(int purchaseId, const Employee& by, const Date& today) {
        PurchaseOrder& order = findPurchase(purchaseId);
        order.markReceived();
        for (const auto& line : order.getLines()) {
            warehouse_.getInventory().receive(line.getIsbn(), line.getQuantity(), by, today);
        }
        log_.log(by.getId(), "принята закупка №" + std::to_string(purchaseId), today);
    }

    BookCatalog& getCatalog() { return catalog_; }
    const BookCatalog& getCatalog() const { return catalog_; }
    Warehouse& getWarehouse() { return warehouse_; }
    const Warehouse& getWarehouse() const { return warehouse_; }
    const std::vector<Supplier>& getSuppliers() const { return suppliers_; }
    const std::vector<Customer>& getCustomers() const { return customers_; }
    const std::vector<CustomerOrder>& getOrders() const { return orders_; }
    const std::vector<PurchaseOrder>& getPurchaseOrders() const { return purchaseOrders_; }
    const ActivityLog& getLog() const { return log_; }
    const PricingPolicy& getPricing() const { return pricing_; }
};
