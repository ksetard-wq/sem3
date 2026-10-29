#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Booking.h"
#include "Notification.h"

class NotificationService {
    std::vector<Notification> notifications_;
    int nextId_ = 1;
    int retentionDays_ = 90;

public:
    NotificationService() = default;
    explicit NotificationService(int retentionDays) : retentionDays_(retentionDays) {}

    int send(const std::string& recipientId, const std::string& message, const Date& date) {
        if (message.empty()) throw InvalidDataException("пустое уведомление");
        notifications_.emplace_back(nextId_, recipientId, message, date);
        return nextId_++;
    }

    void notifyBooking(const Booking& booking, const std::string& event, const Date& date) {
        send(booking.getTravelerId(), booking.type() + " " + booking.getId() + ": " + event, date);
    }

    std::vector<Notification> unreadFor(const std::string& recipientId) const {
        std::vector<Notification> result;
        for (const auto& n : notifications_) {
            if (n.getRecipientId() == recipientId && n.isUnread()) result.push_back(n);
        }
        return result;
    }

    void markAllRead(const std::string& recipientId) {
        for (auto& n : notifications_) {
            if (n.getRecipientId() == recipientId) n.markRead();
        }
    }

    int purgeOld(const Date& today) {
        size_t before = notifications_.size();
        int days = retentionDays_;
        notifications_.erase(std::remove_if(notifications_.begin(), notifications_.end(),
                                            [&](const Notification& n) { return n.isOlderThan(today, days); }),
                             notifications_.end());
        return static_cast<int>(before - notifications_.size());
    }

    int countFor(const std::string& recipientId) const {
        int count = 0;
        for (const auto& n : notifications_) {
            if (n.getRecipientId() == recipientId) ++count;
        }
        return count;
    }

    const std::vector<Notification>& getNotifications() const { return notifications_; }
    int getRetentionDays() const { return retentionDays_; }
};
