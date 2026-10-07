#ifndef MULTISET_H
#define MULTISET_H

#include "Element.h"
#include <cstddef>

/**
 * @file Multiset.h
 * @brief Объявление класса Multiset (неориентированное мультимножество).
 */

/**
 * @class Multiset
 * @brief Класс, представляющий мультимножество, содержащее произвольные элементы Element (Атомы и вложенные Мультимножества).
 */
class Multiset : public Element {
private:
    Element** items; ///< Динамический массив указателей на элементы.
    size_t count;    ///< Текущее количество элементов в мультимножестве.
    size_t capacity; ///< Текущая емкость выделенного массива.

    /**
     * @brief Изменяет размер динамического массива items.
     * @param cap Новая емкость массива.
     */
    void resize(size_t cap);

public:
    /**
     * @brief Конструктор по умолчанию. Создает пустое мультимножество.
     */
    Multiset();

    /**
     * @brief Конструктор копирования (глубокое копирование).
     * @param other Копируемый объект.
     */
    Multiset(const Multiset& other);

    /**
     * @brief Оператор присваивания (глубокое копирование).
     * @param other Объект-источник.
     * @return Ссылка на текущий объект.
     */
    Multiset& operator=(const Multiset& other);

    /**
     * @brief Деструктор. Освобождает всю выделенную память.
     */
    ~Multiset() override;

    /**
     * @brief Полностью очищает мультимножество.
     */
    void clear();

    /**
     * @brief Добавляет копию элемента в мультимножество.
     * @param elem Указатель на добавляемый элемент.
     */
    void add(const Element* elem);

    /**
     * @brief Удаляет одно вхождение элемента из мультимножества.
     * @param elem Указатель на элемент для удаления.
     * @return true, если элемент найден и удален, иначе false.
     */
    bool remove(const Element* elem);

    /**
     * @brief Возвращает количество элементов верхнего уровня.
     * @return Число элементов.
     */
    size_t size() const;

    bool equals(const Element* other) const override;
    void print() const override;
    Element* clone() const override;

    /**
     * @brief Оператор объединения мультимножеств (A += B).
     */
    Multiset& operator+=(const Multiset& other);

    /**
     * @brief Оператор разности мультимножеств (A -= B).
     */
    Multiset& operator-=(const Multiset& other);

    /**
     * @brief Статический метод парсинга строки вида {a, b, {c, d}}.
     * @param str Указатель на C-строку. Перемещается по мере чтения.
     * @return Распознанный объект Multiset.
     */
    static Multiset parse(const char*& str);
};
/**
 * @brief Бинарный оператор объединения двух мультимножеств (A + B).
 */
Multiset operator+(Multiset lhs, const Multiset& rhs);

/**
 * @brief Бинарный оператор разности двух мультимножеств (A - B).
 */
Multiset operator-(Multiset lhs, const Multiset& rhs);
#endif // MULTISET_H