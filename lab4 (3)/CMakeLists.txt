cmake_minimum_required(VERSION 3.10)
project(lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Исходники в кириллице: говорим MSVC читать/писать UTF-8
if (MSVC)
    add_compile_options(/utf-8 /W4)
else ()
    add_compile_options(-Wall -Wextra)
endif ()

# Замер покрытия (gcc/clang): cmake -DCOVERAGE=ON ..
option(COVERAGE "Build tests with coverage" OFF)

# ---------- Программа с консольным меню ----------
add_executable(lab4 lab4/main.cpp)

# ---------- Unit-тесты ----------
add_executable(lab4_tests lab4/tests/unity.cpp)
target_include_directories(lab4_tests PRIVATE lab4)

if (COVERAGE AND NOT MSVC)
    target_compile_options(lab4_tests PRIVATE -O0 --coverage)
    target_link_options(lab4_tests PRIVATE --coverage)
endif ()

enable_testing()
add_test(NAME lab4_tests COMMAND lab4_tests)
