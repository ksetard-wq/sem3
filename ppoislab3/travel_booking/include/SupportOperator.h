#pragma once
#include <string>
#include "Staff.h"
#include "SupportTicket.h"

class SupportOperator : public Staff {
    int openTickets_ = 0;
    int resolvedTotal_ = 0;
    int maxTickets_ = 5;

public:
    SupportOperator(const Staff& base, int maxTickets) : Staff(base), maxTickets_(maxTickets) {}

    bool isOverloaded() const { return openTickets_ >= maxTickets_; }
    double workloadPercent() const { return 100.0 * openTickets_ / maxTickets_; }

    void takeTicket(SupportTicket& ticket) {
        if (isOverloaded()) throw AccessDeniedException(fullName(), "взять ещё одно обращение");
        ticket.assignTo(getId());
        ++openTickets_;
    }

    void replyTo(SupportTicket& ticket, const std::string& text) const { ticket.addMessage(fullName(), text); }

    void resolveTicket(SupportTicket& ticket) {
        ticket.resolve();
        --openTickets_;
        ++resolvedTotal_;
    }

    std::string role() const override { return "Оператор поддержки"; }

    int getOpenTickets() const { return openTickets_; }
    int getResolvedTotal() const { return resolvedTotal_; }
    int getMaxTickets() const { return maxTickets_; }
};
