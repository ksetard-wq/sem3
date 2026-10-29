#include <algorithm>
#include <cstdint>
#include <list>
#include <random>
#include <string>
#include <vector>

#include "../msd_radix_sort.hpp"
#include "../student.hpp"
#include "mini_test.hpp"

using namespace lab;

TEST(radix_empty_and_single) {
    std::vector<int> e;
    msd_radix_sort(e);
    CHECK(e.empty());
    std::vector<int> one{7};
    msd_radix_sort(one);
    CHECK(one == std::vector<int>{7});
}

TEST(radix_signed_ints_with_negatives) {
    std::vector<int> v{5, -3, 0, 100, -100, 5, 2147483647, -2147483647 - 1, 1};
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    msd_radix_sort(v);
    CHECK(v == expected);
}

TEST(radix_descending) {
    std::vector<int> v{5, -3, 0, 100, -100, 5};
    msd_radix_sort(v, order::descending);
    CHECK((v == std::vector<int>{100, 5, 5, 0, -3, -100}));
}

TEST(radix_various_integer_widths) {
    std::vector<std::int8_t> a{5, -128, 127, 0, -1};
    msd_radix_sort(a);
    CHECK((a == std::vector<std::int8_t>{-128, -1, 0, 5, 127}));
    std::vector<std::uint8_t> b{200, 5, 255, 0};
    msd_radix_sort(b);
    CHECK((b == std::vector<std::uint8_t>{0, 5, 200, 255}));
    std::vector<std::int64_t> c{INT64_MAX, INT64_MIN, 0, -1, 1};
    msd_radix_sort(c);
    CHECK((c == std::vector<std::int64_t>{INT64_MIN, -1, 0, 1, INT64_MAX}));
    std::vector<std::uint64_t> d{UINT64_MAX, 0, 1ull << 40, 1};
    msd_radix_sort(d);
    CHECK((d == std::vector<std::uint64_t>{0, 1, 1ull << 40, UINT64_MAX}));
    std::vector<char> e{'z', 'a', 'm'};
    msd_radix_sort(e);
    CHECK((e == std::vector<char>{'a', 'm', 'z'}));
}

TEST(radix_c_array_and_pointer_range) {
    int a[6] = {9, -2, 4, 4, 0, -7};
    msd_radix_sort(a);
    CHECK(std::is_sorted(std::begin(a), std::end(a)));
    int b[4] = {4, 3, 2, 1};
    msd_radix_sort(b, b + 4, order::descending);
    CHECK(b[0] == 4 && b[3] == 1);
}

TEST(radix_forward_iterators) {
    std::list<int> l{3, -1, 2};
    msd_radix_sort(l.begin(), l.end());
    CHECK((l == std::list<int>{-1, 2, 3}));
}

TEST(radix_strings_ascending_and_prefixes) {
    std::vector<std::string> v{"b", "ab", "", "abc", "a", "a", "ba", "Z"};
    msd_radix_sort(v);
    CHECK((v == std::vector<std::string>{"", "Z", "a", "a", "ab", "abc", "b", "ba"}));
}

TEST(radix_strings_descending) {
    std::vector<std::string> v{"b", "ab", "", "abc", "a", "ba"};
    msd_radix_sort(v, order::descending);
    CHECK((v == std::vector<std::string>{"ba", "b", "abc", "ab", "a", ""}));
}

TEST(radix_strings_non_ascii_utf8) {
    std::vector<std::string> v{"яблоко", "арбуз", "груша"};
    msd_radix_sort(v);
    CHECK((v == std::vector<std::string>{"арбуз", "груша", "яблоко"}));
}

TEST(radix_is_stable_with_key) {
    using P = std::pair<int, int>;  // (ключ, исходный номер)
    std::vector<P> v;
    for (int i = 0; i < 60; ++i) v.push_back({(i * 7) % 4, i});
    msd_radix_sort_by(v, [](const P& p) { return p.first; });
    bool ok = true;
    for (std::size_t i = 1; i < v.size(); ++i) {
        if (v[i - 1].first > v[i].first) ok = false;
        if (v[i - 1].first == v[i].first && v[i - 1].second > v[i].second) ok = false;
    }
    CHECK(ok);
    // в убывающем порядке равные тоже остаются в исходном порядке
    std::vector<P> w;
    for (int i = 0; i < 60; ++i) w.push_back({(i * 7) % 4, i});
    msd_radix_sort_by(w, [](const P& p) { return p.first; }, order::descending);
    ok = true;
    for (std::size_t i = 1; i < w.size(); ++i) {
        if (w[i - 1].first < w[i].first) ok = false;
        if (w[i - 1].first == w[i].first && w[i - 1].second > w[i].second) ok = false;
    }
    CHECK(ok);
}

TEST(radix_custom_class_by_string_and_int_keys) {
    std::vector<Student> s{{"Иванов", "A-2", 4.5}, {"Петров", "A-1", 3.8}, {"Алексеев", "A-3", 3.8}};
    msd_radix_sort_by(s, [](const Student& x) { return x.name(); });
    CHECK(s[0].name() == "Алексеев" && s[1].name() == "Иванов" && s[2].name() == "Петров");
    msd_radix_sort_by(s, [](const Student& x) { return x.group(); });
    CHECK(s[0].group() == "A-1" && s[2].group() == "A-3");
    msd_radix_sort_by(s, [](const Student& x) { return static_cast<int>(x.grade() * 100); });
    CHECK(s[2].name() == "Иванов");
    msd_radix_sort_by(s.begin(), s.end(), [](const Student& x) { return x.name().size(); }, order::descending);
    CHECK(s[0].name().size() >= s[2].name().size());
    msd_radix_sort_by(s, [](const Student& x) { return x.name().c_str(); });  // ключ const char*
    CHECK(s[0].name() == "Алексеев");
}

TEST(radix_matches_std_stable_sort_random) {
    std::mt19937 gen(777);
    for (int round = 0; round < 20; ++round) {
        std::vector<int> a(300);
        for (int& x : a) x = static_cast<int>(gen()) % 1000 - 500;
        auto b = a;
        msd_radix_sort(a);
        std::stable_sort(b.begin(), b.end());
        CHECK(a == b);
        msd_radix_sort(a, order::descending);
        std::stable_sort(b.begin(), b.end(), std::greater<int>());
        CHECK(a == b);
    }
    for (int round = 0; round < 10; ++round) {
        std::vector<std::string> a(200);
        for (auto& s : a) {
            s.resize(gen() % 6);
            for (char& c : s) c = static_cast<char>('a' + gen() % 3);
        }
        auto b = a;
        msd_radix_sort(a);
        std::stable_sort(b.begin(), b.end());
        CHECK(a == b);
        msd_radix_sort(a, order::descending);
        std::stable_sort(b.begin(), b.end(), std::greater<std::string>());
        CHECK(a == b);
    }
}

TEST(radix_all_equal_and_long_identical_strings_no_stack_overflow) {
    std::vector<int> v(1000, 42);
    msd_radix_sort(v);
    CHECK(v.size() == 1000 && v.front() == 42);
    std::vector<std::string> s{std::string(50000, 'x'), std::string(50000, 'x'), std::string(49999, 'x')};
    msd_radix_sort(s);
    CHECK(s[0].size() == 49999);
}
