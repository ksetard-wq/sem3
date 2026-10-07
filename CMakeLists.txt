cmake_minimum_required(VERSION 3.20)
project(ppoislab1)

set(CMAKE_CXX_STANDARD 20)


if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()


enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 1. Основное приложение
add_executable(ppoislab1
        main.cpp
        Element.h
        Multiset.h
        Multiset.cpp
)

# 2. Запуск приложения с Мультимножеством
add_executable(multiset_app
        main_multiset.cpp
        Element.h
        Multiset.h
        Multiset.cpp
)

# 3. Запуск игры Крестики-Нолики
add_executable(tictactoe_app
        main_tictactoe.cpp
        TicTacToe.h
        TicTacToe.cpp
)

# 4. Юнит-тесты (GoogleTest)
add_executable(unit_tests
        test_lab.cpp
        Multiset.cpp
        Element.h
        Multiset.h
        TicTacToe.cpp
        TicTacToe.h
)

# Связываем тесты с библиотекой GoogleTest
target_link_libraries(unit_tests PRIVATE gtest_main)

# Автоматическое обнаружение тестов для CTest / CLion
include(GoogleTest)
gtest_discover_tests(unit_tests)