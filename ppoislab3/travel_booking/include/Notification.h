#pragma once
#include <string>
#include <utility>
#include "Date.h"

class Notification {
    int id_ = 0;
    std::string recipientId_;
    std::string message_;
    Date sentAt_;
    bool read_ = false;

public:
    Notification(int id, std::string recipientId, std::string message, Date sentAt)
        : id_(id), recipientId_(std::move(recipientId)), message_(std::move(message)), sentAt_(sentAt) {}

    void markRead() { read_ = true; }
    bool isUnread() const { return !read_; }
    bool isOlderThan(const Date& today, int days) const { return sentAt_.daysUntil(today) > days; }

    std::string preview(size_t length) const {
        if (message_.size() <= length) return message_;
        size_t cut = length;
        while (cut > 0 && (static_cast<unsigned char>(message_[cut]) & 0xC0) == 0x80) --cut;
        return message_.substr(0, cut) + "...";
    }

    int getId() const { return id_; }
    const std::string& getRecipientId() const { return recipientId_; }
    const std::string& getMessage() const { return message_; }
    const Date& getSentAt() const { return sentAt_; }
    bool getRead() const { return read_; }
};
