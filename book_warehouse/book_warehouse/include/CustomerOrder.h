#pragma once
#include <string>
#include <vector>
#include "Address.h"
#include "Customer.h"
#include "Discount.h"
#include "OrderLine.h"
#include "Payment.h"

enum class OrderStatus { New, Confirmed, Packed, Shipped, Delivered, Cancelled };

inline std::string toString(OrderStatus status) {
    switch (status) {
        case OrderStatus::New: return "Новый";
        case OrderStatus::Confirmed: return "Подтверждён";
        case OrderStatus::Packed: return "Упакован";
        case OrderStatus::Shipped: return "Отгружен";
        case OrderStatus::Delivered: return "Доставлен";
        default: return "Отменён";
    }
}

class CustomerOrder {
    int id_ = 0;
    Customer customer_;
    std::vector<OrderLine> lines_;
    OrderStatus status_ = OrderStatus::New;
    Date createdAt_;
    Address shippingAddress_;
    double discountPercent_ = 0.0;
    std::vector<Payment> payments_;

public:
    CustomerOrder(int id, Customer customer, Date createdAt, Address shippingAddress)
        : id_(id), customer_(std::move(customer)), createdAt_(createdAt), shippingAddress_(std::move(shippingAddress)) {}

    void requireStatus(OrderStatus expected, const std::string& action) const {
        if (status_ != expected) throw InvalidOrderStateException(action, toString(status_));
    }

    void addLine(const OrderLine& line) {
        requireStatus(OrderStatus::New, "добавить позицию");
        for (auto& l : lines_) {
            if (l.getIsbn().equals(line.getIsbn())) {
                l.changeQuantity(l.getQuantity() + line.getQuantity());
                return;
            }
        }
        lines_.push_back(line);
    }

    void removeLine(const ISBN& isbn) {
        requireStatus(OrderStatus::New, "удалить позицию");
        for (size_t i = 0; i < lines_.size(); ++i) {
            if (lines_[i].getIsbn().equals(isbn)) {
                lines_.erase(lines_.begin() + static_cast<long>(i));
                return;
            }
        }
        throw BookNotFoundException(isbn.getCode() + " нет в заказе");
    }

    Money subtotal() const {
        Money sum;
        for (const auto& l : lines_) sum = sum.add(l.total());
        return sum;
    }

    void applyDiscount(Discount& discount, const Date& today) {
        discount.applyTo(subtotal(), today);
        discountPercent_ = discount.getPercent();
        discount.registerUse();
    }

    Money total() const { return subtotal().percentOf(100.0 - discountPercent_); }

    void confirm() {
        requireStatus(OrderStatus::New, "подтвердить заказ");
        if (lines_.empty()) throw InvalidDataException("пустой заказ");
        status_ = OrderStatus::Confirmed;
    }

    void pay(const Payment& payment) {
        if (!payment.isSuccessful()) throw PaymentFailedException("платёж не выполнен");
        if (total().isGreaterThan(payment.getAmount())) throw PaymentFailedException("сумма платежа меньше суммы заказа");
        payments_.push_back(payment);
    }

    bool isPaid() const { return !payments_.empty(); }

    void pack() {
        requireStatus(OrderStatus::Confirmed, "упаковать заказ");
        if (!isPaid()) throw PaymentFailedException("заказ не оплачен");
        status_ = OrderStatus::Packed;
    }

    void ship() {
        requireStatus(OrderStatus::Packed, "отгрузить заказ");
        status_ = OrderStatus::Shipped;
    }

    void deliver() {
        requireStatus(OrderStatus::Shipped, "доставить заказ");
        status_ = OrderStatus::Delivered;
    }

    void cancel() {
        if (status_ != OrderStatus::New && status_ != OrderStatus::Confirmed) {
            throw InvalidOrderStateException("отменить заказ", toString(status_));
        }
        status_ = OrderStatus::Cancelled;
    }

    int totalQuantity() const {
        int sum = 0;
        for (const auto& l : lines_) sum += l.getQuantity();
        return sum;
    }

    int getId() const { return id_; }
    const Customer& getCustomer() const { return customer_; }
    const std::vector<OrderLine>& getLines() const { return lines_; }
    OrderStatus getStatus() const { return status_; }
    const Date& getCreatedAt() const { return createdAt_; }
    const Address& getShippingAddress() const { return shippingAddress_; }
    double getDiscountPercent() const { return discountPercent_; }
    const std::vector<Payment>& getPayments() const { return payments_; }
};
