cmake_minimum_required(VERSION 3.10)
project(book_warehouse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(book_warehouse src/main.cpp)
target_include_directories(book_warehouse PRIVATE include)

enable_testing()
add_executable(run_tests tests/tests.cpp)
target_include_directories(run_tests PRIVATE include)
add_test(NAME all_tests COMMAND run_tests)
