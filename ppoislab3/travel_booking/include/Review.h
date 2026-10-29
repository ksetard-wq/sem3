#pragma once
#include <sstream>
#include <string>
#include <utility>
#include "Date.h"

class Review {
    std::string id_;
    std::string authorId_;
    std::string targetId_;
    int rating_ = 5;
    std::string text_;
    Date date_;

public:
    Review(std::string id, std::string authorId, std::string targetId, int rating, std::string text, Date date)
        : id_(std::move(id)), authorId_(std::move(authorId)), targetId_(std::move(targetId)), rating_(rating),
          text_(std::move(text)), date_(date) {
        if (rating < 1 || rating > 5) throw InvalidDataException("оценка отзыва от 1 до 5");
    }

    bool isPositive() const { return rating_ >= 4; }
    bool isNegative() const { return rating_ <= 2; }
    bool isRecent(const Date& today, int days) const { return date_.daysUntil(today) <= days; }

    int wordCount() const {
        std::istringstream in(text_);
        std::string word;
        int count = 0;
        while (in >> word) ++count;
        return count;
    }

    const std::string& getId() const { return id_; }
    const std::string& getAuthorId() const { return authorId_; }
    const std::string& getTargetId() const { return targetId_; }
    int getRating() const { return rating_; }
    const std::string& getText() const { return text_; }
    const Date& getDate() const { return date_; }
};
