#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "cli.hpp"

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    lab::cli::run_menu(std::cin, std::cout);
    return 0;
}
