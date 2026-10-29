#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include "Attraction.h"
#include "Staff.h"

class Guide : public Staff {
    std::vector<std::string> languages_;
    Money hourlyRate_;
    bool certified_ = false;

public:
    Guide() = default;
    Guide(const Staff& base, std::vector<std::string> languages, Money hourlyRate)
        : Staff(base), languages_(std::move(languages)), hourlyRate_(std::move(hourlyRate)) {}

    bool speaks(const std::string& language) const {
        return std::find(languages_.begin(), languages_.end(), language) != languages_.end();
    }

    void addLanguage(const std::string& language) {
        if (!speaks(language)) languages_.push_back(language);
    }

    void certify() { certified_ = true; }
    Money costFor(int hours) const { return hourlyRate_.multiply(hours); }

    bool canGuide(const Attraction& attraction, const std::string& language) const {
        return certified_ && speaks(language) && attraction.openHours() > 0;
    }

    std::string role() const override { return "Гид"; }

    const std::vector<std::string>& getLanguages() const { return languages_; }
    const Money& getHourlyRate() const { return hourlyRate_; }
    bool getCertified() const { return certified_; }
};
