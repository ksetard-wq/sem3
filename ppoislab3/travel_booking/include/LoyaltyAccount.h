#pragma once
#include <string>
#include <utility>
#include "Date.h"
#include "Money.h"

enum class LoyaltyTier { Standard, Silver, Gold, Platinum };

inline std::string toString(LoyaltyTier tier) {
    switch (tier) {
        case LoyaltyTier::Standard: return "Стандарт";
        case LoyaltyTier::Silver: return "Серебро";
        case LoyaltyTier::Gold: return "Золото";
        default: return "Платина";
    }
}

class LoyaltyAccount {
    std::string travelerId_;
    int points_ = 0;
    LoyaltyTier tier_ = LoyaltyTier::Standard;
    Date joinedAt_;

public:
    LoyaltyAccount(std::string travelerId, Date joinedAt) : travelerId_(std::move(travelerId)), joinedAt_(joinedAt) {}

    void earnFromPurchase(const Money& amount) {
        points_ += static_cast<int>(amount.getCents() / 10000);
        recalculateTier();
    }

    void redeem(int points) {
        if (points <= 0) throw InvalidDataException("количество баллов должно быть положительным");
        if (points > points_) throw InsufficientPointsException(points, points_);
        points_ -= points;
        recalculateTier();
    }

    void recalculateTier() {
        if (points_ >= 15000) tier_ = LoyaltyTier::Platinum;
        else if (points_ >= 5000) tier_ = LoyaltyTier::Gold;
        else if (points_ >= 1000) tier_ = LoyaltyTier::Silver;
        else tier_ = LoyaltyTier::Standard;
    }

    double discountPercent() const {
        switch (tier_) {
            case LoyaltyTier::Silver: return 3.0;
            case LoyaltyTier::Gold: return 5.0;
            case LoyaltyTier::Platinum: return 8.0;
            default: return 0.0;
        }
    }

    Money pointsValue() const { return Money::fromUnits(points_); }

    const std::string& getTravelerId() const { return travelerId_; }
    int getPoints() const { return points_; }
    LoyaltyTier getTier() const { return tier_; }
    const Date& getJoinedAt() const { return joinedAt_; }
};
