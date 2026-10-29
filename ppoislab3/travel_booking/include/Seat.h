#pragma once
#include <string>
#include "CabinClass.h"
#include "exceptions.h"

class Seat {
    int row_ = 1;
    char letter_ = 'A';
    CabinClass cabin_ = CabinClass::Economy;
    bool taken_ = false;

public:
    Seat() = default;
    Seat(int row, char letter, CabinClass cabin) : row_(row), letter_(letter), cabin_(cabin) {}

    std::string label() const { return std::to_string(row_) + std::string(1, letter_); }

    void reserve() {
        if (taken_) throw NoSeatsAvailableException("место " + label(), 1, 0);
        taken_ = true;
    }

    void release() { taken_ = false; }
    bool isWindow() const { return letter_ == 'A' || letter_ == 'F'; }
    bool isAisle() const { return letter_ == 'C' || letter_ == 'D'; }

    int getRow() const { return row_; }
    char getLetter() const { return letter_; }
    CabinClass getCabin() const { return cabin_; }
    bool getTaken() const { return taken_; }
};
