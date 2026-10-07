#include "TicTacToe.h"
#include <iostream>
#include <windows.h>
void printGameMenu() {
    std::cout << "\n=== МЕНЮ: КРЕСТИКИ-НОЛИКИ ===\n"
              << "1. Начать новую игру\n"
              << "2. Сделать ход\n"
              << "3. Показать игровое поле\n"
              << "0. Выход\n"
              << "Выберите действие: ";
}

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
    TicTacToe game(3, 3);
    int choice = -1;

    while (choice != 0) {
        printGameMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        if (choice == 1) {
            size_t sz, win;
            std::cout << "Введите размер поля (от 3 до 10) и победную длину: ";
            if (std::cin >> sz >> win) {
                game.reset(sz, win);
                std::cout << "Игра пересоздана.\n";
                game.print();
            }
        } else if (choice == 2) {
            if (game.isFinished()) {
                std::cout << "Текущая партия завершена. Начните новую игру (пункт 1).\n";
                continue;
            }
            game.print();
            std::cout << "Ход игрока (" << (game.getCurrentPlayer() == Cell::Cross ? "X" : "O") << ").\n";
            std::cout << "Введите номер строки и столбца через пробел: ";
            size_t r, c;
            if (std::cin >> r >> c) {
                if (!game.makeMove(r, c)) {
                    std::cout << "Ошибка! Некорректные координаты или ячейка уже занята.\n";
                } else {
                    game.print();
                }
            }
        } else if (choice == 3) {
            game.print();
        }
    }
    return 0;
}