#include "Multiset.h"
#include <iostream>
#include <cstring>
#include <cctype>

Atom::Atom(const char* s) {
    std::strncpy(val, s, 31);
    val[31] = '\0';
}

bool Atom::equals(const Element* other) const {
    auto a = dynamic_cast<const Atom*>(other);
    return a && std::strcmp(val, a->val) == 0;
}

void Atom::print() const {
    std::cout << val;
}

Element* Atom::clone() const {
    return new Atom(val);
}

const char* Atom::get() const {
    return val;
}

Multiset::Multiset() : items(nullptr), count(0), capacity(0) {}

void Multiset::resize(size_t cap) {
    Element** new_items = new Element*[cap];
    for (size_t i = 0; i < count; ++i) new_items[i] = items[i];
    delete[] items;
    items = new_items;
    capacity = cap;
}

Multiset::Multiset(const Multiset& other) : items(nullptr), count(0), capacity(0) {
    resize(other.count);
    for (size_t i = 0; i < other.count; ++i) {
        items[i] = other.items[i]->clone();
    }
    count = other.count;
}

Multiset& Multiset::operator=(const Multiset& other) {
    if (this != &other) {
        clear();
        resize(other.count);
        for (size_t i = 0; i < other.count; ++i) {
            items[i] = other.items[i]->clone();
        }
        count = other.count;
    }
    return *this;
}

Multiset::~Multiset() {
    clear();
}

void Multiset::clear() {
    for (size_t i = 0; i < count; ++i) delete items[i];
    delete[] items;
    items = nullptr;
    count = capacity = 0;
}

void Multiset::add(const Element* elem) {
    if (count == capacity) resize(capacity == 0 ? 4 : capacity * 2);
    items[count++] = elem->clone();
}

bool Multiset::remove(const Element* elem) {
    for (size_t i = 0; i < count; ++i) {
        if (items[i]->equals(elem)) {
            delete items[i];
            for (size_t j = i; j < count - 1; ++j) items[j] = items[j + 1];
            count--;
            return true;
        }
    }
    return false;
}

size_t Multiset::size() const { return count; }

bool Multiset::equals(const Element* other) const {
    auto m = dynamic_cast<const Multiset*>(other);
    if (!m || count != m->count) return false;

    // Выделяем память динамически, чтобы не завязываться на фиксированный размер 128
    bool* used = new bool[m->count]{false};

    for (size_t i = 0; i < count; ++i) {
        bool found = false;
        for (size_t j = 0; j < m->count; ++j) {
            if (!used[j] && items[i]->equals(m->items[j])) {
                used[j] = true;
                found = true;
                break;
            }
        }
        if (!found) {
            delete[] used;
            return false;
        }
    }

    delete[] used;
    return true;
}

void Multiset::print() const {
    std::cout << "{";
    for (size_t i = 0; i < count; ++i) {
        items[i]->print();
        if (i + 1 < count) std::cout << ", ";
    }
    std::cout << "}";
}

Element* Multiset::clone() const {
    return new Multiset(*this);
}

Multiset& Multiset::operator+=(const Multiset& other) {
    for (size_t i = 0; i < other.count; ++i) add(other.items[i]);
    return *this;
}

Multiset& Multiset::operator-=(const Multiset& other) {
    for (size_t i = 0; i < other.count; ++i) remove(other.items[i]);
    return *this;
}

Multiset Multiset::parse(const char*& str) {
    Multiset set;

    // Пропускаем начальные пробелы
    while (*str && std::isspace(*str)) str++;

    // Если парсинг начинается с '{', прогоняем символ '{'
    if (*str == '{') {
        str++;
    }

    while (*str) {
        // Пропускаем пробелы
        while (*str && std::isspace(*str)) str++;

        if (*str == '\0') {
            break;
        }

        if (*str == '}') {
            str++; // Пропускаем закрывающую фигурную скобку
            break; // Завершаем парсинг текущего (вложенного) множества
        }

        if (*str == ',') {
            str++; // Пропускаем запятую-разделитель
            continue;
        }

        if (*str == '{') {
            // Вложенное множество: создаем его через рекурсивный вызов
            Multiset child = parse(str);
            set.add(&child);
        } else {
            // Атом (обычный элемент/строка/число)
            char buf[32] = {0};
            size_t idx = 0;
            while (*str && !std::isspace(*str) && *str != ',' && *str != '}' && *str != '{') {
                if (idx < 31) buf[idx++] = *str;
                str++;
            }
            if (idx > 0) {
                Atom a(buf);
                set.add(&a);
            }
        }
    }

    return set;
}
// Добавьте этот блок в конец файла Multiset.cpp

Multiset operator+(Multiset lhs, const Multiset& rhs) {
    lhs += rhs;
    return lhs;
}

Multiset operator-(Multiset lhs, const Multiset& rhs) {
    lhs -= rhs;
    return lhs;
}