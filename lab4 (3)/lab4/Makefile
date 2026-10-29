CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

all: lab4 test_runner

lab4: main.cpp cli.hpp binary_tree_sort.hpp directed_graph.hpp student.hpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp

test_runner: tests/unity.cpp $(wildcard tests/*.cpp) $(wildcard *.hpp)
	$(CXX) $(CXXFLAGS) -o $@ tests/unity.cpp

test: test_runner
	./test_runner

# покрытие: make coverage  (нужен gcov)
coverage:
	$(CXX) -std=c++17 -O0 --coverage -o cov_runner tests/unity.cpp
	./cov_runner
	gcov -o . cov_runner-unity.gcda | grep -A1 -E "File '[^']*\.hpp'"

clean:
	rm -f lab4 test_runner cov_runner* *.gcda *.gcno *.gcov
