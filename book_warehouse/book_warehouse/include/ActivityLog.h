#pragma once
#include <string>
#include <vector>
#include "Date.h"

class ActivityLog {
    std::vector<std::string> entries_;
    size_t capacity_ = 1000;

public:
    ActivityLog() = default;
    explicit ActivityLog(size_t capacity) : capacity_(capacity) {}

    void log(const std::string& who, const std::string& action, const Date& date) {
        if (entries_.size() >= capacity_) entries_.erase(entries_.begin());
        entries_.push_back(date.toString() + " [" + who + "] " + action);
    }

    std::vector<std::string> entriesBy(const std::string& who) const {
        std::vector<std::string> result;
        for (const auto& e : entries_) {
            if (e.find("[" + who + "]") != std::string::npos) result.push_back(e);
        }
        return result;
    }

    std::vector<std::string> lastEntries(size_t count) const {
        size_t start = entries_.size() > count ? entries_.size() - count : 0;
        return std::vector<std::string>(entries_.begin() + static_cast<long>(start), entries_.end());
    }

    void clear() { entries_.clear(); }
    size_t size() const { return entries_.size(); }

    const std::vector<std::string>& getEntries() const { return entries_; }
};
