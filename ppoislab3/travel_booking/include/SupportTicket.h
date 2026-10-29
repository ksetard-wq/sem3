#pragma once
#include <string>
#include <utility>
#include <vector>
#include "Date.h"

enum class TicketStatus { Open, InProgress, Resolved, Closed };

inline std::string toString(TicketStatus status) {
    switch (status) {
        case TicketStatus::Open: return "Открыт";
        case TicketStatus::InProgress: return "В работе";
        case TicketStatus::Resolved: return "Решён";
        default: return "Закрыт";
    }
}

class SupportTicket {
    int id_ = 0;
    std::string travelerId_;
    std::string bookingId_;
    std::string subject_;
    TicketStatus status_ = TicketStatus::Open;
    Date createdAt_;
    std::vector<std::string> messages_;
    std::string assignedTo_;

public:
    SupportTicket(int id, std::string travelerId, std::string bookingId, std::string subject, Date createdAt)
        : id_(id), travelerId_(std::move(travelerId)), bookingId_(std::move(bookingId)), subject_(std::move(subject)),
          createdAt_(createdAt) {
        if (subject_.empty()) throw InvalidDataException("тема обращения не указана");
    }

    void addMessage(const std::string& author, const std::string& text) {
        if (status_ == TicketStatus::Closed) throw InvalidStateException("добавить сообщение", toString(status_));
        messages_.push_back(author + ": " + text);
    }

    void assignTo(const std::string& operatorId) {
        if (status_ != TicketStatus::Open) throw InvalidStateException("назначить оператора", toString(status_));
        assignedTo_ = operatorId;
        status_ = TicketStatus::InProgress;
    }

    void resolve() {
        if (status_ != TicketStatus::InProgress) throw InvalidStateException("решить обращение", toString(status_));
        status_ = TicketStatus::Resolved;
    }

    void close() {
        if (status_ != TicketStatus::Resolved) throw InvalidStateException("закрыть обращение", toString(status_));
        status_ = TicketStatus::Closed;
    }

    void reopen() {
        if (status_ != TicketStatus::Resolved) throw InvalidStateException("переоткрыть обращение", toString(status_));
        status_ = TicketStatus::InProgress;
    }

    bool isOpen() const { return status_ == TicketStatus::Open || status_ == TicketStatus::InProgress; }
    int ageDays(const Date& today) const { return createdAt_.daysUntil(today); }
    bool isStale(const Date& today, int days) const { return isOpen() && ageDays(today) > days; }

    int getId() const { return id_; }
    const std::string& getTravelerId() const { return travelerId_; }
    const std::string& getBookingId() const { return bookingId_; }
    const std::string& getSubject() const { return subject_; }
    TicketStatus getStatus() const { return status_; }
    const Date& getCreatedAt() const { return createdAt_; }
    const std::vector<std::string>& getMessages() const { return messages_; }
    const std::string& getAssignedTo() const { return assignedTo_; }
};
