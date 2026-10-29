// Задание 1. Обобщённая сортировка "MSD Radix sort" (вариант 8, вторая сортировка).
//
// MSD (most significant digit) radix sort — поразрядная сортировка, которая
// идёт от СТАРШЕГО разряда к младшему:
//   1. Раскладываем элементы по "корзинам" по старшему разряду ключа
//      (поразрядная сортировка подсчётом, она устойчива).
//   2. Каждую корзину, где больше одного элемента, сортируем так же по
//      следующему разряду, и т. д.
// Разряд = один байт (основание 256).
//
// Как это "обобщённо". Поразрядная сортировка не сравнивает элементы, а
// смотрит на цифры ключа, поэтому для произвольного типа T нужна функция
// ключа key(T) -> ключ. Поддерживаются два вида ключей:
//   * целые числа (знаковые и беззнаковые, любой ширины) — фиксированное число
//     разрядов = sizeof(ключа); у знаковых переворачивается знаковый бит,
//     чтобы отрицательные шли раньше положительных;
//   * строки (std::string, std::string_view, const char*) — разряд = символ,
//     более короткая строка (кончившаяся раньше) идёт первой.
// Для пользовательского класса достаточно передать лямбду, достающую ключ.
//
// Свойства:
//   * устойчивая сортировка (равные ключи сохраняют исходный порядок);
//   * без рекурсии (явный стек задач) — длинные одинаковые строки не
//     переполняют стек;
//   * сложность O(n * L), где L — длина ключа в байтах; доп. память O(n);
//   * работает с ForwardIterator, типу T не нужен конструктор по умолчанию.
#pragma once

#include <cstddef>
#include <iterator>
#include <limits>
#include <numeric>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace lab {

enum class order { ascending, descending };

namespace detail {

template <class>
inline constexpr bool always_false = false;

// 0..257: 0 — "ключ кончился" (для строк по возрастанию), 1..256 — байт,
// 257 — "ключ кончился" (для строк по убыванию).
constexpr std::size_t radix_buckets = 258;

struct task {
    std::size_t lo, hi, pos;  // диапазон в массиве индексов и номер разряда
};

// Ядро: по ключам возвращает перестановку индексов, дающую отсортированный порядок.
// digit(key, pos) -> номер корзины; корзина end_bucket означает "ключ кончился"
// (в неё не заходим глубже); max_pos — максимальное число разрядов.
template <class Key, class Digit>
std::vector<std::size_t> msd_order(const std::vector<Key>& keys, std::size_t max_pos, std::size_t end_bucket,
                                   Digit digit) {
    const std::size_t n = keys.size();
    std::vector<std::size_t> idx(n), aux(n);
    std::iota(idx.begin(), idx.end(), std::size_t{0});

    std::vector<task> stack;
    if (n > 1) stack.push_back({0, n, 0});

    while (!stack.empty()) {
        const task t = stack.back();
        stack.pop_back();

        // count[b+1] — сколько элементов в корзине b; после префиксных сумм
        // count[b] — начало корзины b, count[b+1] — её конец.
        std::size_t count[radix_buckets + 1] = {0};
        for (std::size_t i = t.lo; i < t.hi; ++i) ++count[digit(keys[idx[i]], t.pos) + 1];
        for (std::size_t b = 0; b < radix_buckets; ++b) count[b + 1] += count[b];

        std::size_t next[radix_buckets];
        for (std::size_t b = 0; b < radix_buckets; ++b) next[b] = count[b];
        for (std::size_t i = t.lo; i < t.hi; ++i) aux[t.lo + next[digit(keys[idx[i]], t.pos)]++] = idx[i];
        for (std::size_t i = t.lo; i < t.hi; ++i) idx[i] = aux[i];

        if (t.pos + 1 >= max_pos) continue;  // разряды кончились
        for (std::size_t b = 0; b < radix_buckets; ++b) {
            if (b == end_bucket) continue;  // там все ключи равны, сортировать нечего
            if (count[b + 1] - count[b] > 1) stack.push_back({t.lo + count[b], t.lo + count[b + 1], t.pos + 1});
        }
    }
    return idx;
}

// Целый ключ -> беззнаковый с сохранением порядка (у знаковых переворачиваем старший бит).
template <class K>
auto to_unsigned_key(K k) {
    using U = std::make_unsigned_t<K>;
    U u = static_cast<U>(k);
    if constexpr (std::is_signed_v<K>) u ^= static_cast<U>(U{1} << (sizeof(U) * 8 - 1));
    return u;
}

struct identity_key {
    template <class T>
    const T& operator()(const T& x) const {
        return x;
    }
};

}  // namespace detail

// Сортировка диапазона [first, last) по ключу key(element).
// key должен возвращать целое число (кроме bool) или строку.
template <class It, class KeyFn>
void msd_radix_sort_by(It first, It last, KeyFn key, order ord = order::ascending) {
    using T = typename std::iterator_traits<It>::value_type;

    std::vector<T> items;
    for (It it = first; it != last; ++it) items.push_back(std::move(*it));
    if (items.size() < 2) {  // 0 или 1 элемент: просто вернуть на место
        It out = first;
        for (auto& x : items) *out++ = std::move(x);
        return;
    }

    using Key = std::decay_t<decltype(key(items.front()))>;
    const bool asc = ord == order::ascending;
    std::vector<std::size_t> idx;

    if constexpr (std::is_integral_v<Key> && !std::is_same_v<Key, bool>) {
        using U = std::make_unsigned_t<Key>;
        constexpr std::size_t width = sizeof(U);
        std::vector<U> keys;
        keys.reserve(items.size());
        for (const T& x : items) keys.push_back(detail::to_unsigned_key<Key>(key(x)));
        idx = detail::msd_order(keys, width, detail::radix_buckets, [asc](U k, std::size_t pos) -> std::size_t {
            const std::size_t byte = static_cast<std::size_t>((k >> (8 * (width - 1 - pos))) & 0xFFu);
            return asc ? byte + 1 : 256 - byte;
        });
    } else if constexpr (std::is_convertible_v<Key, std::string_view>) {
        std::vector<std::string> keys;
        keys.reserve(items.size());
        for (const T& x : items) keys.emplace_back(key(x));
        const std::size_t end_bucket = asc ? 0 : 257;
        idx = detail::msd_order(keys, std::numeric_limits<std::size_t>::max(), end_bucket,
                                [asc, end_bucket](const std::string& s, std::size_t pos) -> std::size_t {
                                    if (pos >= s.size()) return end_bucket;
                                    const std::size_t c = static_cast<unsigned char>(s[pos]);
                                    return asc ? c + 1 : 256 - c;
                                });
    } else {
        static_assert(detail::always_false<Key>,
                      "msd_radix_sort: ключ должен быть целым числом или строкой; для своего типа используйте msd_radix_sort_by с функцией ключа");
    }

    It out = first;
    for (std::size_t i : idx) *out++ = std::move(items[i]);
}

// Сортировка самих элементов (целые числа, std::string).
template <class It>
void msd_radix_sort(It first, It last, order ord = order::ascending) {
    msd_radix_sort_by(first, last, detail::identity_key{}, ord);
}

// Версии для контейнеров и C-массивов.
template <class Container>
auto msd_radix_sort(Container& c, order ord = order::ascending) -> decltype(std::begin(c), std::end(c), void()) {
    msd_radix_sort(std::begin(c), std::end(c), ord);
}

template <class Container, class KeyFn>
auto msd_radix_sort_by(Container& c, KeyFn key, order ord = order::ascending)
    -> decltype(std::begin(c), std::end(c), void()) {
    msd_radix_sort_by(std::begin(c), std::end(c), key, ord);
}

}  // namespace lab
