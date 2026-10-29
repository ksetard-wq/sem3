// Консольное меню: демонстрация сортировки и графа.
// Меню принимает потоки ввода/вывода, поэтому его можно тестировать без консоли.
#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "binary_tree_sort.hpp"
#include "directed_graph.hpp"
#include "msd_radix_sort.hpp"
#include "student.hpp"

namespace lab {
namespace cli {

struct input_closed {};  // поток ввода закончился

using Graph = DirectedGraph<std::string>;

inline std::string ask(std::istream& in, std::ostream& out, const std::string& prompt) {
    out << prompt;
    std::string s;
    if (!std::getline(in, s)) throw input_closed{};
    return s;
}

inline std::size_t ask_id(std::istream& in, std::ostream& out, const std::string& prompt) {
    const std::string s = ask(in, out, prompt);
    if (s.empty() || !std::isdigit(static_cast<unsigned char>(s[0])))
        throw std::invalid_argument("нужно целое неотрицательное число");
    try {
        return static_cast<std::size_t>(std::stoull(s));
    } catch (const std::exception&) {
        throw std::invalid_argument("нужно целое неотрицательное число");
    }
}

template <class V>
void print_vec(std::ostream& out, const V& v) {
    out << "[";
    bool first = true;
    for (const auto& x : v) {
        out << (first ? "" : ", ") << x;
        first = false;
    }
    out << "]\n";
}

// ----------------------------- сортировка -----------------------------
// Разбор строки с целыми числами. true — вся строка разобрана без ошибок.
inline bool parse_ints(const std::string& line, std::vector<int>& v) {
    std::istringstream ss(line);
    int x;
    while (ss >> x) v.push_back(x);
    return ss.eof();
}

// Ввод количества элементов (с защитой от слишком больших значений).
inline std::size_t ask_count(std::istream& in, std::ostream& out, const std::string& prompt) {
    const std::size_t n = ask_id(in, out, prompt);
    if (n > 100000) throw std::invalid_argument("слишком много элементов (максимум 100000)");
    return n;
}

enum class Algo { tree, radix };

inline const char* algo_name(Algo a) { return a == Algo::tree ? "Binary tree sort" : "MSD Radix sort"; }

inline void sort_numbers(std::istream& in, std::ostream& out, Algo algo) {
    std::vector<int> v;
    if (!parse_ints(ask(in, out, "Числа через пробел: "), v)) {
        out << "Ошибка: ожидались целые числа\n";
        return;
    }
    const bool desc = ask(in, out, "По убыванию? (y/n): ") == "y";
    out << "До:    ";
    print_vec(out, v);
    if (algo == Algo::tree) {
        if (desc) binary_tree_sort(v, std::greater<int>());
        else binary_tree_sort(v);
    } else {
        msd_radix_sort(v, desc ? order::descending : order::ascending);
    }
    out << "После: ";
    print_vec(out, v);
}

// Обычный C-массив: размер задаёт пользователь, память выделяется динамически.
inline void sort_c_array(std::istream& in, std::ostream& out, Algo algo) {
    const std::size_t n = ask_count(in, out, "Размер массива: ");
    std::vector<int> input;
    if (!parse_ints(ask(in, out, "Элементы через пробел: "), input)) {
        out << "Ошибка: ожидались целые числа\n";
        return;
    }
    if (input.size() != n) {
        out << "Ошибка: ожидалось элементов: " << n << ", введено: " << input.size() << "\n";
        return;
    }
    std::unique_ptr<int[]> arr(new int[n]);
    std::copy(input.begin(), input.end(), arr.get());
    out << "До:    ";
    print_vec(out, input);
    if (algo == Algo::tree) binary_tree_sort(arr.get(), arr.get() + n);  // диапазон указателей — тоже итераторы
    else msd_radix_sort(arr.get(), arr.get() + n);
    out << "После: ";
    print_vec(out, std::vector<int>(arr.get(), arr.get() + n));
}

inline void sort_strings(std::istream& in, std::ostream& out, Algo algo) {
    std::vector<std::string> w;
    {
        std::istringstream ss(ask(in, out, "Слова через пробел: "));
        std::string t;
        while (ss >> t) w.push_back(t);
    }
    out << "Порядок: 1-по длине 2-по алфавиту 3-по длине (убывание) 4-по алфавиту (убывание)\n";
    const std::string mode = ask(in, out, "> ");
    if (mode != "1" && mode != "2" && mode != "3" && mode != "4") {
        out << "Неизвестный вариант\n";
        return;
    }
    out << "До:    ";
    print_vec(out, w);
    if (algo == Algo::tree) {
        if (mode == "1") binary_tree_sort(w, [](const std::string& a, const std::string& b) { return a.size() < b.size(); });
        else if (mode == "2") binary_tree_sort(w);
        else if (mode == "3") binary_tree_sort(w, [](const std::string& a, const std::string& b) { return a.size() > b.size(); });
        else binary_tree_sort(w, std::greater<std::string>());
    } else {
        // для поразрядной сортировки вместо компаратора задаётся функция КЛЮЧА
        auto len = [](const std::string& x) { return x.size(); };
        if (mode == "1") msd_radix_sort_by(w, len);
        else if (mode == "2") msd_radix_sort(w);
        else if (mode == "3") msd_radix_sort_by(w, len, order::descending);
        else msd_radix_sort(w, order::descending);
    }
    out << "После: ";
    print_vec(out, w);
    if (mode == "1" || mode == "3") out << "(сортировка устойчива: слова одной длины сохранили порядок ввода)\n";
}

inline void sort_students(std::istream& in, std::ostream& out, Algo algo) {
    const std::size_t n = ask_count(in, out, "Сколько студентов: ");
    std::vector<Student> s;
    while (s.size() < n) {
        std::istringstream ss(ask(in, out, "Студент " + std::to_string(s.size() + 1) + " (имя группа балл): "));
        std::string name, group, rest;
        double grade;
        if (ss >> name >> group >> grade && !(ss >> rest)) {
            s.emplace_back(name, group, grade);
        } else {
            out << "Ошибка: нужно три значения, например: Иванов A-1 4.5\n";
        }
    }
    if (s.empty()) {
        out << "Список пуст\n";
        return;
    }
    if (algo == Algo::tree) out << "Сортировать по: 1-баллу (порядок по умолчанию: балл, имя) 2-имени 3-группе\n";
    else out << "Сортировать по: 1-баллу (ключ: балл*100 как целое) 2-имени 3-группе\n";
    const std::string mode = ask(in, out, "> ");
    if (mode != "1" && mode != "2" && mode != "3") {
        out << "Неизвестный вариант\n";
        return;
    }
    if (algo == Algo::tree) {
        if (mode == "1") binary_tree_sort(s);  // используется Student::operator<
        else if (mode == "2") binary_tree_sort(s, [](const Student& a, const Student& b) { return a.name() < b.name(); });
        else binary_tree_sort(s, [](const Student& a, const Student& b) { return a.group() < b.group(); });
    } else {
        if (mode == "1") msd_radix_sort_by(s, [](const Student& x) { return static_cast<long long>(std::llround(x.grade() * 100)); });
        else if (mode == "2") msd_radix_sort_by(s, [](const Student& x) { return x.name(); });
        else msd_radix_sort_by(s, [](const Student& x) { return x.group(); });
    }
    out << "Результат:\n";
    for (const auto& x : s) out << "  " << x << "\n";
}

inline void sort_items_menu(std::istream& in, std::ostream& out, Algo algo) {
    for (;;) {
        out << "\n--- " << algo_name(algo) << " ---------\n"
               "1. Отсортировать введённые числа \n"
               "2. Отсортировать массив \n"
               "3. Отсортировать введённые строки\n"
               "4. Отсортировать объекты класса Student \n"
               "0. Назад\n";
        const std::string c = ask(in, out, "> ");
        try {
            if (c == "0") return;
            else if (c == "1") sort_numbers(in, out, algo);
            else if (c == "2") sort_c_array(in, out, algo);
            else if (c == "3") sort_strings(in, out, algo);
            else if (c == "4") sort_students(in, out, algo);
            else out << "Неизвестный пункт меню\n";
        } catch (const std::invalid_argument& e) {
            out << "Ошибка ввода: " << e.what() << "\n";
        }
    }
}

inline void sort_menu(std::istream& in, std::ostream& out) {
    for (;;) {
        out << "\n--- Сортировка: выбор алгоритма ---\n"
               "1. Binary tree sort\n"
               "2. MSD Radix sort\n"
               "0. Назад\n";
        const std::string c = ask(in, out, "> ");
        if (c == "0") return;
        else if (c == "1") sort_items_menu(in, out, Algo::tree);
        else if (c == "2") sort_items_menu(in, out, Algo::radix);
        else out << "Неизвестный пункт меню\n";
    }
}

// ----------------------------- граф -----------------------------
inline std::string describe(const Graph::Vertex& v) { return std::to_string(v.id) + ":" + v.value; }
inline std::string describe(const Graph::Edge& e) { return std::to_string(e.from) + "->" + std::to_string(e.to); }

template <class It>
void print_range(std::ostream& out, It b, It e) {
    for (; b != e; ++b) out << " " << describe(*b);
    out << "\n";
}

inline void iterators_demo(Graph& g, std::istream& in, std::ostream& out) {
    out << "Что перебирать: 1-вершины 2-рёбра 3-рёбра, инцидентные вершине 4-смежные вершины\n";
    const std::string kind = ask(in, out, "> ");
    out << "Вид итератора: 1-обычный 2-const 3-reverse 4-const reverse\n";
    const std::string mode = ask(in, out, "> ");
    if (kind == "1") {
        if (mode == "1") print_range(out, g.begin_vertices(), g.end_vertices());
        else if (mode == "2") print_range(out, g.cbegin_vertices(), g.cend_vertices());
        else if (mode == "3") print_range(out, g.rbegin_vertices(), g.rend_vertices());
        else print_range(out, g.crbegin_vertices(), g.crend_vertices());
    } else if (kind == "2") {
        if (mode == "1") print_range(out, g.begin_edges(), g.end_edges());
        else if (mode == "2") print_range(out, g.cbegin_edges(), g.cend_edges());
        else if (mode == "3") print_range(out, g.rbegin_edges(), g.rend_edges());
        else print_range(out, g.crbegin_edges(), g.crend_edges());
    } else if (kind == "3") {
        const auto v = ask_id(in, out, "id вершины: ");
        if (mode == "1") print_range(out, g.begin_incident(v), g.end_incident(v));
        else if (mode == "2") print_range(out, g.cbegin_incident(v), g.cend_incident(v));
        else if (mode == "3") print_range(out, g.rbegin_incident(v), g.rend_incident(v));
        else print_range(out, g.crbegin_incident(v), g.crend_incident(v));
    } else if (kind == "4") {
        const auto v = ask_id(in, out, "id вершины: ");
        if (mode == "1") print_range(out, g.begin_adjacent(v), g.end_adjacent(v));
        else if (mode == "2") print_range(out, g.cbegin_adjacent(v), g.cend_adjacent(v));
        else if (mode == "3") print_range(out, g.rbegin_adjacent(v), g.rend_adjacent(v));
        else print_range(out, g.crbegin_adjacent(v), g.crend_adjacent(v));
    } else {
        out << "Неизвестный вариант\n";
    }
}

inline void fill_demo(Graph& g) {
    g.clear();
    const auto a = g.add_vertex("A"), b = g.add_vertex("B"), c = g.add_vertex("C"), d = g.add_vertex("D");
    g.add_edge(a, b);
    g.add_edge(a, c);
    g.add_edge(b, c);
    g.add_edge(c, a);
    g.add_edge(d, d);
}

inline void graph_menu(std::istream& in, std::ostream& out) {
    Graph g;
    for (;;) {
        out << "\n--- Ориентированный граф (упорядоченные списки рёбер) ---\n"
               " 1. Добавить вершину          2. Добавить ребро\n"
               " 3. Удалить вершину           4. Удалить ребро\n"
               " 5. Показать граф             6. Степень вершины\n"
               " 7. Степень ребра             8. Проверить наличие вершины/ребра\n"
               " 9. Перебор итераторами      10. Удалить вершину по итератору\n"
               "11. Удалить ребро по итератору 12. Копия и сравнение графов\n"
               "13. Заполнить демо-графом    14. Очистить\n"
               " 0. Назад\n";
        const std::string c = ask(in, out, "> ");
        try {
            if (c == "0") return;
            else if (c == "1") {
                const auto id = g.add_vertex(ask(in, out, "Значение вершины: "));
                out << "Добавлена вершина id=" << id << "\n";
            } else if (c == "2") {
                const auto a = ask_id(in, out, "Откуда (id): ");
                const auto b = ask_id(in, out, "Куда (id): ");
                g.add_edge(a, b);
                out << "Ребро добавлено\n";
            } else if (c == "3") {
                g.remove_vertex(ask_id(in, out, "id вершины: "));
                out << "Вершина удалена\n";
            } else if (c == "4") {
                const auto a = ask_id(in, out, "Откуда (id): ");
                const auto b = ask_id(in, out, "Куда (id): ");
                g.remove_edge(a, b);
                out << "Ребро удалено\n";
            } else if (c == "5") {
                out << g << "\nВершин: " << g.vertex_count() << ", рёбер: " << g.edge_count() << (g.empty() ? " (граф пуст)" : "") << "\n";
            } else if (c == "6") {
                const auto v = ask_id(in, out, "id вершины: ");
                out << "степень=" << g.degree(v) << " (исходящих " << g.out_degree(v) << ", входящих " << g.in_degree(v) << ")\n";
            } else if (c == "7") {
                const auto a = ask_id(in, out, "Откуда (id): ");
                const auto b = ask_id(in, out, "Куда (id): ");
                out << "степень ребра=" << g.edge_degree(a, b) << "\n";
            } else if (c == "8") {
                out << (g.has_vertex(ask_id(in, out, "id вершины: ")) ? "вершина есть\n" : "вершины нет\n");
                const auto a = ask_id(in, out, "Ребро, откуда (id): ");
                const auto b = ask_id(in, out, "Ребро, куда (id): ");
                out << (g.has_edge(a, b) ? "ребро есть\n" : "ребра нет\n");
            } else if (c == "9") {
                iterators_demo(g, in, out);
            } else if (c == "10") {
                const std::string val = ask(in, out, "Значение вершины для удаления: ");
                auto it = std::find_if(g.begin_vertices(), g.end_vertices(), [&](const Graph::Vertex& v) { return v.value == val; });
                if (it == g.end_vertices()) out << "Не найдена\n";
                else {
                    it = g.erase_vertex(it);
                    out << "Удалена; следующая вершина: " << (it == g.end_vertices() ? "end()" : describe(*it)) << "\n";
                }
            } else if (c == "11") {
                const auto a = ask_id(in, out, "Откуда (id): ");
                const auto b = ask_id(in, out, "Куда (id): ");
                auto it = g.find_edge(a, b);
                if (it == g.end_edges()) out << "Не найдено\n";
                else {
                    it = g.erase_edge(it);
                    out << "Удалено; следующее ребро: " << (it == g.end_edges() ? "end()" : describe(*it)) << "\n";
                }
            } else if (c == "12") {
                Graph h(g);
                out << "Копия равна оригиналу: " << (h == g ? "да" : "нет") << "\n";
                h.add_vertex("extra");
                out << "После добавления вершины в копию: ==" << (h == g) << " !=" << (h != g) << " <" << (h < g) << " >" << (h > g)
                    << " <=" << (h <= g) << " >=" << (h >= g) << "\n";
                g = h;
                out << "Присваивание g = h выполнено, равны: " << (g == h ? "да" : "нет") << "\n";
            } else if (c == "13") {
                fill_demo(g);
                out << "Демо-граф: " << g << "\n";
            } else if (c == "14") {
                g.clear();
                out << "Граф очищен\n";
            } else {
                out << "Неизвестный пункт меню\n";
            }
        } catch (const graph_error& e) {
            out << "Ошибка графа: " << e.what() << "\n";
        } catch (const std::invalid_argument& e) {
            out << "Ошибка ввода: " << e.what() << "\n";
        }
    }
}

// ----------------------------- главное меню -----------------------------
inline void run_menu(std::istream& in, std::ostream& out) {
    try {
        for (;;) {
            out <<
                   "1. Сортировка (Binary tree sort / MSD Radix sort)\n"
                   "2. Граф \n"
                   "0. Выход\n";
            const std::string c = ask(in, out, "> ");
            if (c == "0") return;
            if (c == "1") sort_menu(in, out);
            else if (c == "2") graph_menu(in, out);
            else out << "Неизвестный пункт меню\n";
        }
    } catch (const input_closed&) {
        out << "\nВвод завершён.\n";
    }
}

}  // namespace cli
}  // namespace lab
