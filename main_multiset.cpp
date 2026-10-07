#include "Multiset.h"
#include <iostream>
#include <windows.h>
void printMenu() {
    std::cout << "\n=== МЕНЮ: МУЛЬТИМНОЖЕСТВО ===\n"
              << "1. Ввести мультимножество A из строки\n"
              << "2. Ввести мультимножество B из строки\n"
              << "3. Показать текущие A и B\n"
              << "4. Объединить: A += B\n"
              << "5. Разность: A -= B\n"
              << "6. Проверить равенство (A == B)\n"
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
    Multiset A, B;
    char buffer[256];
    int choice = -1;

    while (choice != 0) {
        printMenu();
        if (!(std::cout, std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        std::cin.ignore(1000, '\n');

        if (choice == 1) {
            std::cout << "Введите A (например {a, a, {b, c}}): ";
            std::cin.getline(buffer, 256);
            const char* ptr = buffer;
            A = Multiset::parse(ptr);
            std::cout << "Мультимножество A успешно распознано.\n";
        } else if (choice == 2) {
            std::cout << "Введите B (например {a, b}): ";
            std::cin.getline(buffer, 256);
            const char* ptr = buffer;
            B = Multiset::parse(ptr);
            std::cout << "Мультимножество B успешно распознано.\n";
        } else if (choice == 3) {
            std::cout << "A = "; A.print(); std::cout << "\n";
            std::cout << "B = "; B.print(); std::cout << "\n";
        } else if (choice == 4) {
            A += B;
            std::cout << "Результат A += B: "; A.print(); std::cout << "\n";
        } else if (choice == 5) {
            A -= B;
            std::cout << "Результат A -= B: "; A.print(); std::cout << "\n";
        } else if (choice == 6) {
            if (A.equals(&B)) {
                std::cout << "Мультимножества A и B равны.\n";
            } else {
                std::cout << "Мультимножества A и B НЕ равны.\n";
            }
        }
    }
    return 0;
}