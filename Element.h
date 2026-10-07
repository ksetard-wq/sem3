#ifndef ELEMENT_H
#define ELEMENT_H

/**
 * @file Element.h
 * @brief Объявление базового абстрактного класса Element и производного класса Atom.
 */

/**
 * @class Element
 * @brief Абстрактный базовый класс для всех элементов мультимножества.
 */
class Element {
public:
    virtual ~Element() = default;

    /**
     * @brief Сравнивает текущий элемент с другим элементом.
     * @param other Указатель на элемент для сравнения.
     * @return true, если элементы равны, иначе false.
     */
    virtual bool equals(const Element* other) const = 0;

    /**
     * @brief Выводит текстовое представление элемента в поток std::cout.
     */
    virtual void print() const = 0;

    /**
     * @brief Создает точную глубокую копию элемента в динамической памяти.
     * @return Указатель на созданный объект.
     */
    virtual Element* clone() const = 0;
};

/**
 * @class Atom
 * @brief Класс, представляющий атомарный элемент (строку без пробелов).
 */
class Atom : public Element {
private:
    char val[32]; ///< Символьный буфер фиксированного размера для хранения значения атома.

public:
    /**
     * @brief Конструктор атома из C-строки.
     * @param s Входная C-строка.
     */
    explicit Atom(const char* s);

    bool equals(const Element* other) const override;
    void print() const override;
    Element* clone() const override;

    /**
     * @brief Геттер для получения значения атома.
     * @return Указатель на C-строку с хранимым значением.
     */
    const char* get() const;
};

#endif // ELEMENT_H