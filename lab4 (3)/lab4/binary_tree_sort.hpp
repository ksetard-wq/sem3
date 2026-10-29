
#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

namespace lab {

// Основная версия: диапазон итераторов. Нужен как минимум ForwardIterator
// (диапазон проходим дважды: чтение и запись результата).
template <class It, class Compare = std::less<>>
void binary_tree_sort(It first, It last, Compare comp = Compare()) {
    using T = typename std::iterator_traits<It>::value_type;
    constexpr std::size_t npos = static_cast<std::size_t>(-1);

    struct Node {
        T value;
        std::size_t left;
        std::size_t right;
    };

    std::vector<Node> nodes;

    // --- Фаза 1: построение дерева ---
    for (It it = first; it != last; ++it) {
        const std::size_t idx = nodes.size();
        nodes.push_back(Node{std::move(*it), npos, npos});
        if (idx == 0) continue;  // корень
        std::size_t cur = 0;
        for (;;) {
            if (comp(nodes[idx].value, nodes[cur].value)) {  // строго меньше -> влево
                if (nodes[cur].left == npos) { nodes[cur].left = idx; break; }
                cur = nodes[cur].left;
            } else {                                          // больше или равно -> вправо
                if (nodes[cur].right == npos) { nodes[cur].right = idx; break; }
                cur = nodes[cur].right;
            }
        }
    }

    // --- Фаза 2: симметричный обход с явным стеком, запись обратно в диапазон ---
    std::vector<std::size_t> stack;
    std::size_t cur = nodes.empty() ? npos : 0;
    It out = first;
    while (!stack.empty() || cur != npos) {
        while (cur != npos) {
            stack.push_back(cur);
            cur = nodes[cur].left;
        }
        cur = stack.back();
        stack.pop_back();
        *out = std::move(nodes[cur].value);
        ++out;
        cur = nodes[cur].right;
    }
}

// Версия для контейнеров и C-массивов (T arr[N]): std::begin/std::end работают с обоими.
template <class Container, class Compare = std::less<>>
auto binary_tree_sort(Container& c, Compare comp = Compare())
    -> decltype(std::begin(c), std::end(c), void()) {
    binary_tree_sort(std::begin(c), std::end(c), comp);
}

// Проверка упорядоченности (для тестов и демонстрации).
template <class It, class Compare = std::less<>>
bool is_sorted_range(It first, It last, Compare comp = Compare()) {
    if (first == last) return true;
    It prev = first;
    for (++first; first != last; ++first, ++prev) {
        if (comp(*first, *prev)) return false;
    }
    return true;
}

}  // namespace lab
