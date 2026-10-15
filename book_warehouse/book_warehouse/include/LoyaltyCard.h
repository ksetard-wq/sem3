#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"

enum class LoyaltyTier { Bronze, Silver, Gold };

inline std::string toString(LoyaltyTier tier) {
    switch (tier) {
        case LoyaltyTier::Bronze: return "Бронза";
        case LoyaltyTier::Silver: return "Серебро";
        default: return "Золото";
    }
}

class LoyaltyCard {
    std::string number_;
    int points_ = 0;
    LoyaltyTier tier_ = LoyaltyTier::Bronze;
    Date issuedAt_;

public:
    LoyaltyCard() = default;
    LoyaltyCard(std::string number, Date issuedAt) : number_(std::move(number)), issuedAt_(issuedAt) {}

    void addPoints(const Money& purchase) {
        points_ += static_cast<int>(purchase.getCents() / 10000);
        recalculateTier();
    }

    void spendPoints(int points) {
        if (points <= 0 || points > points_) throw InvalidDataException("недостаточно баллов");
        points_ -= points;
        recalculateTier();
    }

    void recalculateTier() {
        if (points_ >= 2000) tier_ = LoyaltyTier::Gold;
        else if (points_ >= 500) tier_ = LoyaltyTier::Silver;
        else tier_ = LoyaltyTier::Bronze;
    }

    double discountPercent() const {
        switch (tier_) {
            case LoyaltyTier::Gold: return 5.0;
            case LoyaltyTier::Silver: return 3.0;
            default: return 0.0;
        }
    }

    const std::string& getNumber() const { return number_; }
    int getPoints() const { return points_; }
    LoyaltyTier getTier() const { return tier_; }
    const Date& getIssuedAt() const { return issuedAt_; }
};
