#include <sstream>
#include <string>

#include "../cli.hpp"
#include "mini_test.hpp"

using namespace lab;

namespace {
std::string run(const std::string& script) {
    std::istringstream in(script);
    std::ostringstream out;
    cli::run_menu(in, out);
    return out.str();
}
bool has(const std::string& text, const std::string& part) { return text.find(part) != std::string::npos; }
}  // namespace



namespace {
// Сценарий: главное меню -> сортировка -> алгоритм algo -> items -> выходы из всех меню
std::string sort_run(const std::string& algo, const std::string& items) {
    return run("1\n" + algo + "\n" + items + "0\n0\n0\n");
}
}  // namespace

TEST(cli_sort_algorithm_menu) {
    CHECK(has(run("1\n9\n0\n0\n"), "Неизвестный пункт меню"));
    CHECK(has(sort_run("1", "9\n"), "Неизвестный пункт меню"));
    CHECK(has(sort_run("1", ""), "Binary tree sort ---"));
    CHECK(has(sort_run("2", ""), "MSD Radix sort ---"));
}

TEST(cli_sort_numbers_both_algorithms) {
    for (const char* algo : {"1", "2"}) {
        auto o = sort_run(algo, "1\n5 3 9 1\nn\n1\n5 3 9 1\ny\n");
        CHECK(has(o, "После: [1, 3, 5, 9]"));
        CHECK(has(o, "После: [9, 5, 3, 1]"));
        CHECK(has(sort_run(algo, "1\n5 abc\n"), "Ошибка: ожидались целые числа"));
    }
    CHECK(has(sort_run("2", "1\n3 -5 0 -1\nn\n"), "После: [-5, -1, 0, 3]"));
}

TEST(cli_sort_c_array_both_algorithms) {
    for (const char* algo : {"1", "2"}) {
        CHECK(has(sort_run(algo, "2\n5\n3 -1 4 1 5\n"), "После: [-1, 1, 3, 4, 5]"));
        CHECK(has(sort_run(algo, "2\n3\n1 2\n"), "ожидалось элементов: 3, введено: 2"));
        CHECK(has(sort_run(algo, "2\n2\n1 x\n"), "ожидались целые числа"));
        CHECK(has(sort_run(algo, "2\nabc\n"), "Ошибка ввода"));
        CHECK(has(sort_run(algo, "2\n100001\n"), "слишком много"));
        CHECK(has(sort_run(algo, "2\n0\n\n"), "После: []"));
    }
}

TEST(cli_sort_strings_both_algorithms) {
    const std::string w = "3\npear fig banana kiwi apple plum\n";
    for (const char* algo : {"1", "2"}) {
        CHECK(has(sort_run(algo, w + "1\n"), "После: [fig, pear, kiwi, plum, apple, banana]"));
        CHECK(has(sort_run(algo, w + "2\n"), "После: [apple, banana, fig, kiwi, pear, plum]"));
        CHECK(has(sort_run(algo, w + "3\n"), "После: [banana, apple, pear, kiwi, plum, fig]"));
        CHECK(has(sort_run(algo, w + "4\n"), "После: [plum, pear, kiwi, fig, banana, apple]"));
        CHECK(has(sort_run(algo, w + "9\n"), "Неизвестный вариант"));
        CHECK(has(sort_run(algo, w + "1\n"), "устойчива"));
    }
}

TEST(cli_sort_students_both_algorithms) {
    const std::string in = "4\n3\nИванов A-1 4.5\nПетров A-2 3.8\nбад\nАлексеев A-3 3.8\n";
    for (const char* algo : {"1", "2"}) {
        auto by_grade = sort_run(algo, in + "1\n");
        CHECK(has(by_grade, "Ошибка: нужно три значения"));
        CHECK(has(sort_run(algo, in + "2\n"), "  Алексеев (A-3, 3.8)\n  Иванов (A-1, 4.5)\n  Петров (A-2, 3.8)\n"));
        CHECK(has(sort_run(algo, in + "3\n"), "  Иванов (A-1, 4.5)\n  Петров (A-2, 3.8)\n  Алексеев (A-3, 3.8)\n"));
        CHECK(has(sort_run(algo, in + "9\n"), "Неизвестный вариант"));
        CHECK(has(sort_run(algo, "4\n0\n"), "Список пуст"));
        CHECK(has(sort_run(algo, "4\nx\n"), "Ошибка ввода"));
    }
    // по баллу: дерево — (балл, имя), поразрядная — устойчиво по баллу (порядок ввода у равных)
    CHECK(has(sort_run("1", in + "1\n"), "  Алексеев (A-3, 3.8)\n  Петров (A-2, 3.8)\n  Иванов (A-1, 4.5)\n"));
    CHECK(has(sort_run("2", in + "1\n"), "  Петров (A-2, 3.8)\n  Алексеев (A-3, 3.8)\n  Иванов (A-1, 4.5)\n"));
}

TEST(cli_graph_basic_flow) {
    auto o = run("2\n1\nA\n1\nB\n2\n0\n1\n5\n6\n0\n7\n0\n1\n8\n0\n0\n1\n8\n1\n0\n0\n0\n");
    CHECK(has(o, "Добавлена вершина id=0"));
    CHECK(has(o, "Ребро добавлено"));
    CHECK(has(o, "Graph{V=[0:A, 1:B], E=[0->1]}"));
    CHECK(has(o, "степень=1 (исходящих 1, входящих 0)"));
    CHECK(has(o, "степень ребра=0"));
    CHECK(has(o, "вершина есть"));
    CHECK(has(o, "ребро есть"));
    CHECK(has(o, "ребра нет"));
}

TEST(cli_graph_removal_and_errors) {
    auto o = run("2\n13\n4\n0\n1\n3\n3\n4\n7\n7\n2\nx\n2\n99999999999999999999999\n2\n\n0\n0\n");
    CHECK(has(o, "Ребро удалено"));
    CHECK(has(o, "Вершина удалена"));
    CHECK(has(o, "Ошибка графа"));
    CHECK(has(o, "Ошибка ввода"));
}

TEST(cli_graph_iterators_all_modes) {
    std::string s = "2\n13\n";
    for (int kind = 1; kind <= 4; ++kind)
        for (int mode = 1; mode <= 4; ++mode) {
            s += "9\n" + std::to_string(kind) + "\n" + std::to_string(mode) + "\n";
            if (kind >= 3) s += "0\n";
        }
    s += "9\n7\n1\n0\n0\n";
    auto o = run(s);
    CHECK(has(o, " 0:A 1:B 2:C 3:D\n"));
    CHECK(has(o, " 3:D 2:C 1:B 0:A\n"));
    CHECK(has(o, " 0->1 0->2 1->2 2->0 3->3\n"));
    CHECK(has(o, " 2->0 0->2 0->1\n"));
    CHECK(has(o, " 1:B 2:C\n"));
    CHECK(has(o, " 2:C 1:B\n"));
    CHECK(has(o, "Неизвестный вариант"));
}

TEST(cli_graph_erase_by_iterator) {
    auto o = run("2\n13\n10\nB\n10\nnope\n11\n0\n2\n11\n0\n2\n11\n3\n3\n0\n0\n");
    CHECK(has(o, "Удалена; следующая вершина: 2:C"));
    CHECK(has(o, "Не найдена"));
    CHECK(has(o, "Не найдено"));
    CHECK(has(o, "Удалено; следующее ребро: 2->0"));
    o = run("2\n13\n10\nD\n11\n2\n0\n0\n0\n");
    CHECK(has(o, "следующая вершина: end()"));
    o = run("2\n13\n11\n3\n3\n0\n0\n");
    CHECK(has(o, "следующее ребро: end()"));
}

TEST(cli_graph_copy_and_clear) {
    auto o = run("2\n12\n13\n12\n14\n5\n0\n0\n");
    CHECK(has(o, "Копия равна оригиналу: да"));
    CHECK(has(o, "Присваивание g = h выполнено"));
    CHECK(has(o, "Граф очищен"));
    CHECK(has(o, "(граф пуст)"));
}
