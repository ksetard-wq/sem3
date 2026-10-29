#include <algorithm>
#include <array>
#include <deque>
#include <list>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "../binary_tree_sort.hpp"
#include "../student.hpp"
#include "mini_test.hpp"

using namespace lab;

TEST(sort_empty_and_single) {
    std::vector<int> e;
    binary_tree_sort(e);
    CHECK(e.empty());
    std::vector<int> one{5};
    binary_tree_sort(one);
    CHECK(one == std::vector<int>{5});
}

TEST(sort_ints_vector) {
    std::vector<int> v{5, 2, 9, -1, 2, 0, 7};
    binary_tree_sort(v);
    CHECK((v == std::vector<int>{-1, 0, 2, 2, 5, 7, 9}));
}

TEST(sort_descending_with_comparator) {
    std::vector<int> v{5, 2, 9, -1, 2};
    binary_tree_sort(v, std::greater<int>());
    CHECK((v == std::vector<int>{9, 5, 2, 2, -1}));
}

TEST(sort_c_array) {
    double a[5] = {3.5, -1.0, 2.25, 0.0, 3.5};
    binary_tree_sort(a);
    CHECK(is_sorted_range(std::begin(a), std::end(a)));
    CHECK(a[0] == -1.0 && a[4] == 3.5);
}

TEST(sort_iterator_range_partial) {
    std::vector<int> v{9, 8, 7, 6, 5, 4};
    binary_tree_sort(v.begin() + 1, v.begin() + 5);  // сортируется только середина
    CHECK((v == std::vector<int>{9, 5, 6, 7, 8, 4}));
}

TEST(sort_other_containers) {
    std::deque<int> d{3, 1, 2};
    binary_tree_sort(d);
    CHECK((d == std::deque<int>{1, 2, 3}));
    std::list<int> l{3, 1, 2};  // ForwardIterator достаточно
    binary_tree_sort(l.begin(), l.end());
    CHECK((l == std::list<int>{1, 2, 3}));
    std::array<char, 4> ar{'d', 'a', 'c', 'b'};
    binary_tree_sort(ar);
    CHECK((ar == std::array<char, 4>{'a', 'b', 'c', 'd'}));
}

TEST(sort_strings) {
    std::vector<std::string> v{"pear", "apple", "fig"};
    binary_tree_sort(v);
    CHECK((v == std::vector<std::string>{"apple", "fig", "pear"}));
}

TEST(sort_is_stable) {
    using P = std::pair<int, int>;  // (ключ, исходный номер)
    std::vector<P> v;
    for (int i = 0; i < 50; ++i) v.push_back({i % 3, i});
    binary_tree_sort(v, [](const P& a, const P& b) { return a.first < b.first; });
    bool stable = true;
    for (std::size_t i = 1; i < v.size(); ++i)
        if (v[i - 1].first == v[i].first && v[i - 1].second > v[i].second) stable = false;
    CHECK(is_sorted_range(v.begin(), v.end(), [](const P& a, const P& b) { return a.first < b.first; }));
    CHECK(stable);
}

TEST(sort_matches_std_sort_random) {
    std::mt19937 gen(12345);
    for (int round = 0; round < 20; ++round) {
        std::vector<int> a(200);
        for (int& x : a) x = static_cast<int>(gen() % 50);
        auto b = a;
        binary_tree_sort(a);
        std::sort(b.begin(), b.end());
        CHECK(a == b);
    }
}

TEST(sort_already_sorted_large_no_stack_overflow) {
    std::vector<int> v(20000);
    std::iota(v.begin(), v.end(), 0);  // худший случай: вырожденное дерево глубиной n
    binary_tree_sort(v);
    CHECK(is_sorted_range(v.begin(), v.end()));
}

TEST(sort_student_objects) {
    std::vector<Student> s{{"Иванов", "A-1", 4.5}, {"Петров", "A-2", 3.8}, {"Алексеев", "A-3", 3.8}};
    binary_tree_sort(s);
    CHECK(s[0].name() == "Алексеев");
    CHECK(s[1].name() == "Петров");
    CHECK(s[2].name() == "Иванов");
    binary_tree_sort(s, [](const Student& a, const Student& b) { return a.name() > b.name(); });
    CHECK(s[0].name() == "Петров");
}

TEST(student_basics) {
    Student a("X", "G", 4.0), b("X", "G", 4.0), c;
    CHECK(a == b);
    CHECK(!(a < b));
    CHECK(c.grade() == 0.0 && c.group().empty());
    std::ostringstream os;
    os << a;
    CHECK(os.str() == "X (G, 4)");
}

TEST(is_sorted_range_works) {
    std::vector<int> a{1, 2, 3}, b{1, 3, 2}, e;
    CHECK(is_sorted_range(a.begin(), a.end()));
    CHECK(!is_sorted_range(b.begin(), b.end()));
    CHECK(is_sorted_range(e.begin(), e.end()));
}
