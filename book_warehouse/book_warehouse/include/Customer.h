#pragma once
#include <vector>
#include "Address.h"
#include "LoyaltyCard.h"
#include "Person.h"

class Customer : public Person {
    Address address_;
    LoyaltyCard loyaltyCard_;
    bool blocked_ = false;
    std::vector<int> orderIds_;

public:
    Customer(std::string id, std::string firstName, std::string lastName, ContactInfo contact, Date birthDate,
             Address address, LoyaltyCard loyaltyCard)
        : Person(std::move(id), std::move(firstName), std::move(lastName), std::move(contact), birthDate),
          address_(std::move(address)), loyaltyCard_(std::move(loyaltyCard)) {}

    void registerOrder(int orderId) { orderIds_.push_back(orderId); }
    void block() { blocked_ = true; }
    void unblock() { blocked_ = false; }
    bool canOrder() const { return !blocked_ && address_.isComplete(); }
    bool isVip() const { return loyaltyCard_.getTier() == LoyaltyTier::Gold; }
    void updateAddress(const Address& address) { address_ = address; }
    void earnPoints(const Money& purchase) { loyaltyCard_.addPoints(purchase); }
    size_t orderCount() const { return orderIds_.size(); }

    const Address& getAddress() const { return address_; }
    const LoyaltyCard& getLoyaltyCard() const { return loyaltyCard_; }
    bool getBlocked() const { return blocked_; }
};
