#pragma once
#include <string>
#include <vector>
#include "Review.h"
#include "utils.h"

class ReviewBoard {
    std::vector<Review> reviews_;
    std::vector<std::string> bannedWords_;

public:
    ReviewBoard() : bannedWords_{"спам", "http"} {}

    void submit(const Review& review) {
        for (const auto& w : bannedWords_) {
            if (containsIgnoreCase(review.getText(), w)) throw InvalidDataException("отзыв содержит запрещённое слово \"" + w + "\"");
        }
        for (const auto& r : reviews_) {
            if (r.getAuthorId() == review.getAuthorId() && r.getTargetId() == review.getTargetId()) {
                throw DuplicateEntityException("отзыв автора " + review.getAuthorId() + " об объекте " + review.getTargetId());
            }
        }
        reviews_.push_back(review);
    }

    int countFor(const std::string& targetId) const {
        int count = 0;
        for (const auto& r : reviews_) {
            if (r.getTargetId() == targetId) ++count;
        }
        return count;
    }

    double averageFor(const std::string& targetId) const {
        int count = countFor(targetId);
        if (count == 0) return 0.0;
        double sum = 0.0;
        for (const auto& r : reviews_) {
            if (r.getTargetId() == targetId) sum += r.getRating();
        }
        return sum / count;
    }

    double positiveShare(const std::string& targetId) const {
        int count = countFor(targetId);
        if (count == 0) return 0.0;
        int positive = 0;
        for (const auto& r : reviews_) {
            if (r.getTargetId() == targetId && r.isPositive()) ++positive;
        }
        return 100.0 * positive / count;
    }

    std::vector<Review> recentFor(const std::string& targetId, const Date& today, int days) const {
        std::vector<Review> result;
        for (const auto& r : reviews_) {
            if (r.getTargetId() == targetId && r.isRecent(today, days)) result.push_back(r);
        }
        return result;
    }

    const std::vector<Review>& getReviews() const { return reviews_; }
};
