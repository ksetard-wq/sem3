#pragma once
#include <cstdio>
#include <string>
#include "exceptions.h"

class Date {
    int year_ = 2000;
    int month_ = 1;
    int day_ = 1;

public:
    Date() = default;
    Date(int year, int month, int day) : year_(year), month_(month), day_(day) {
        if (!isValidDate(year, month, day)) {
            throw InvalidDateException(std::to_string(year) + "-" + std::to_string(month) + "-" + std::to_string(day));
        }
    }

    static bool isLeapYear(int year) { return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0; }

    static int daysInMonth(int year, int month) {
        static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (month == 2 && isLeapYear(year)) return 29;
        return days[month - 1];
    }

    static bool isValidDate(int year, int month, int day) {
        if (year < 1 || month < 1 || month > 12 || day < 1) return false;
        return day <= daysInMonth(year, month);
    }

    int toDayNumber() const {
        int y = year_ - (month_ <= 2 ? 1 : 0);
        int era = (y >= 0 ? y : y - 399) / 400;
        int yoe = y - era * 400;
        int mp = (month_ + 9) % 12;
        int doy = (153 * mp + 2) / 5 + day_ - 1;
        int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + doe - 719468;
    }

    static Date fromDayNumber(int z) {
        z += 719468;
        int era = (z >= 0 ? z : z - 146096) / 146097;
        int doe = z - era * 146097;
        int yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        int y = yoe + era * 400;
        int doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        int mp = (5 * doy + 2) / 153;
        int d = doy - (153 * mp + 2) / 5 + 1;
        int m = mp < 10 ? mp + 3 : mp - 9;
        return Date(y + (m <= 2 ? 1 : 0), m, d);
    }

    int daysUntil(const Date& other) const { return other.toDayNumber() - toDayNumber(); }
    bool isBefore(const Date& other) const { return toDayNumber() < other.toDayNumber(); }
    Date addDays(int days) const { return fromDayNumber(toDayNumber() + days); }

    std::string toString() const {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year_, month_, day_);
        return buf;
    }

    int getYear() const { return year_; }
    int getMonth() const { return month_; }
    int getDay() const { return day_; }
};
