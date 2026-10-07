#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <cstddef>

/**
 * @file TicTacToe.h
 * @brief Объявление класса TicTacToe (Крестики-Нолики) и перечисления Cell.
 */

/**
 * @enum Cell
 * @brief Состояние ячейки игрового поля.
 */
enum class Cell { Empty = 0, Cross = 1, Nought = 2 };

/**
 * @class TicTacToe
 * @brief Класс, реализующий логику и состояние игры Крестики-Нолики произвольного размера.
 */
class TicTacToe {
private:
    Cell board[10][10]; ///< Игровое поле фиксированного максимального размера 10x10.
    size_t size;        ///< Текущий выбранный размер доски.
    size_t win_len;     ///< Длина непрерывной линии для победы.
    Cell current;       ///< Игрок, делающий текущий ход (Cross или Nought).
    int moves;          ///< Общее количество сделанных ходов.
    bool finished;      ///< Флаг завершения игры (победа или ничья).

    /**
     * @brief Проверяет наличие победной линии после хода в ячейку (r, c).
     * @param r Строка последнего хода.
     * @param c Столбец последнего хода.
     * @return true, если текущий игрок победил, иначе false.
     */
    bool checkWin(size_t r, size_t c);

public:
    /**
     * @brief Конструктор игры.
     * @param sz Размер стороны поля (от 3 до 10).
     * @param win Необходимое количество знаков в ряд для победы.
     */
    explicit TicTacToe(size_t sz = 3, size_t win = 3);

    /**
     * @brief Геттер для получения значения ячейки.
     */
    Cell getCell(size_t r, size_t c) const;

    /**
     * @brief Геттер для получения текущего игрока.
     */
    Cell getCurrentPlayer() const;

    /**
     * @brief Проверка, завершена ли партия.
     */
    bool isFinished() const;

    /**
     * @brief Геттер для получения размера поля.
     */
    size_t getSize() const;

    /**
     * @brief Пытается сделать ход текущего игрока в ячейку (r, c).
     * @param r Индекс строки (0-based).
     * @param c Индекс столбца (0-based).
     * @return true, если ход был валидным и выполнен, иначе false.
     */
    bool makeMove(size_t r, size_t c);

    /**
     * @brief Печатает текущее игровое поле в консоль.
     */
    void print() const;

    /**
     * @brief Сбрасывает игру к начальному состоянию с новыми параметрами.
     */
    void reset(size_t sz, size_t win);
};

#endif // TICTACTOE_H