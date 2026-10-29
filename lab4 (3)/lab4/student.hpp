// Пользовательский класс для демонстрации сортировки (Задание 1).
#pragma once

#include <ostream>
#include <string>
#include <tuple>

namespace lab {

class Student {
public:
    Student() = default;
    Student(std::string name, std::string group, double grade)
        : name_(std::move(name)), group_(std::move(group)), grade_(grade) {}

    const std::string& name() const { return name_; }
    const std::string& group() const { return group_; }
    double grade() const { return grade_; }

    // Порядок по умолчанию: средний балл, при равенстве — имя.
    // Именно operator< использует std::less<> в binary_tree_sort.
    friend bool operator<(const Student& a, const Student& b) {
        return std::tie(a.grade_, a.name_) < std::tie(b.grade_, b.name_);
    }
    friend bool operator==(const Student& a, const Student& b) {
        return std::tie(a.name_, a.group_, a.grade_) == std::tie(b.name_, b.group_, b.grade_);
    }
    friend std::ostream& operator<<(std::ostream& os, const Student& s) {
        return os << s.name_ << " (" << s.group_ << ", " << s.grade_ << ")";
    }

private:
    std::string name_;
    std::string group_;
    double grade_ = 0.0;
};

}  // namespace lab
