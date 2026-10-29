#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "../directed_graph.hpp"
#include "mini_test.hpp"

using namespace lab;
using G = DirectedGraph<std::string>;
using Pairs = std::vector<std::pair<std::size_t, std::size_t>>;

namespace {
// Граф: вершины A=0 B=1 C=2 D=3; рёбра 0->1, 0->2, 1->2, 2->0, 3->3 (петля)
G make() {
    G g;
    auto a = g.add_vertex("A"), b = g.add_vertex("B"), c = g.add_vertex("C"), d = g.add_vertex("D");
    g.add_edge(a, b);
    g.add_edge(a, c);
    g.add_edge(b, c);
    g.add_edge(c, a);
    g.add_edge(d, d);
    return g;
}
template <class It>
Pairs edges_of(It b, It e) {
    Pairs r;
    for (; b != e; ++b) r.push_back({b->from, b->to});
    return r;
}
template <class It>
std::string values_of(It b, It e) {
    std::string r;
    for (; b != e; ++b) r += b->value;
    return r;
}
}  // namespace

TEST(graph_default_is_empty) {
    G g;
    CHECK(g.empty());
    CHECK(g.vertex_count() == 0 && g.edge_count() == 0);
    CHECK(g.begin_vertices() == g.end_vertices());
    CHECK(g.begin_edges() == g.end_edges());
}

TEST(graph_counts_and_presence) {
    G g = make();
    CHECK(!g.empty());
    CHECK(g.vertex_count() == 4 && g.edge_count() == 5);
    CHECK(g.has_vertex(0) && g.has_vertex(3) && !g.has_vertex(4));
    CHECK(g.has_edge(0, 1) && g.has_edge(3, 3));
    CHECK(!g.has_edge(1, 0));   // ориентированность
    CHECK(!g.has_edge(9, 0) && !g.has_edge(0, 9));
    CHECK(g.value(2) == "C");
    g.value(2) = "Z";
    CHECK(static_cast<const G&>(g).value(2) == "Z");
}

TEST(graph_degrees) {
    G g = make();
    CHECK(g.out_degree(0) == 2 && g.in_degree(0) == 1 && g.degree(0) == 3);
    CHECK(g.degree(1) == 2);
    CHECK(g.degree(2) == 3);
    CHECK(g.degree(3) == 2);  // петля считается дважды
    CHECK(g.in_degree(3) == 1 && g.out_degree(3) == 1);
    CHECK_THROWS(g.degree(42), vertex_not_found);
}

TEST(graph_edge_degree) {
    G g = make();
    CHECK(g.edge_degree(0, 1) == 3);  // (0,2) (2,0) (1,2)
    CHECK(g.edge_degree(3, 3) == 0);
    CHECK(g.edge_degree(2, 0) == 3);
    CHECK_THROWS(g.edge_degree(1, 0), edge_not_found);
    CHECK_THROWS(g.edge_degree(9, 0), vertex_not_found);
    CHECK_THROWS(g.edge_degree(0, 9), vertex_not_found);
}

TEST(graph_add_errors) {
    G g = make();
    CHECK_THROWS(g.add_edge(0, 1), duplicate_edge);
    CHECK_THROWS(g.add_edge(3, 3), duplicate_edge);
    CHECK_THROWS(g.add_edge(0, 9), vertex_not_found);
    CHECK_THROWS(g.add_edge(9, 0), vertex_not_found);
    CHECK_THROWS(g.add_edge(0, 9), graph_error);  // иерархия исключений
    CHECK(g.edge_count() == 5);
}

TEST(graph_remove_edge) {
    G g = make();
    g.remove_edge(0, 1);
    CHECK(!g.has_edge(0, 1) && g.edge_count() == 4);
    g.remove_edge(3, 3);
    CHECK(!g.has_edge(3, 3) && g.degree(3) == 0);
    CHECK_THROWS(g.remove_edge(0, 1), edge_not_found);
    CHECK_THROWS(g.remove_edge(9, 1), vertex_not_found);
}

TEST(graph_remove_vertex) {
    G g = make();
    g.remove_vertex(2);  // убирает 0->2, 1->2, 2->0
    CHECK(g.vertex_count() == 3 && g.edge_count() == 2);
    CHECK(!g.has_vertex(2) && !g.has_edge(0, 2));
    CHECK(g.degree(0) == 1 && g.degree(1) == 1);
    g.remove_vertex(3);  // петля
    CHECK(g.edge_count() == 1);
    CHECK_THROWS(g.remove_vertex(3), vertex_not_found);
    auto id = g.add_vertex("new");  // id не переиспользуется
    CHECK(id == 4);
}

TEST(graph_clear) {
    G g = make();
    g.clear();
    CHECK(g.empty() && g.edge_count() == 0);
    CHECK(g.add_vertex("x") == 4);
}

TEST(graph_vertex_iterators) {
    G g = make();
    CHECK(values_of(g.begin_vertices(), g.end_vertices()) == "ABCD");
    CHECK(values_of(g.cbegin_vertices(), g.cend_vertices()) == "ABCD");
    CHECK(values_of(g.rbegin_vertices(), g.rend_vertices()) == "DCBA");
    CHECK(values_of(g.crbegin_vertices(), g.crend_vertices()) == "DCBA");
    const G& cg = g;
    CHECK(values_of(cg.begin_vertices(), cg.end_vertices()) == "ABCD");
    CHECK(values_of(cg.rbegin_vertices(), cg.rend_vertices()) == "DCBA");
    for (auto it = g.begin_vertices(); it != g.end_vertices(); ++it) it->value += "!";  // изменение через итератор
    CHECK(g.value(0) == "A!");
    auto it = g.end_vertices();
    --it;                                   // двунаправленность
    CHECK(it->value == "D!");
    CHECK(g.find_vertex(1)->value == "B!");
    CHECK(g.find_vertex(77) == g.end_vertices());
    CHECK(cg.find_vertex(1)->value == "B!");
    CHECK(cg.find_vertex(77) == cg.end_vertices());
}

TEST(graph_edge_iterators) {
    G g = make();
    Pairs fwd{{0, 1}, {0, 2}, {1, 2}, {2, 0}, {3, 3}};
    CHECK(edges_of(g.begin_edges(), g.end_edges()) == fwd);
    CHECK(edges_of(g.cbegin_edges(), g.cend_edges()) == fwd);
    Pairs rev(fwd.rbegin(), fwd.rend());
    CHECK(edges_of(g.rbegin_edges(), g.rend_edges()) == rev);
    CHECK(edges_of(g.crbegin_edges(), g.crend_edges()) == rev);
    const G& cg = g;
    CHECK(edges_of(cg.begin_edges(), cg.end_edges()) == fwd);
    CHECK(edges_of(cg.rbegin_edges(), cg.rend_edges()) == rev);
    // ++/-- в обе стороны, пост-формы, ->, преобразование iterator -> const_iterator
    auto it = g.begin_edges();
    auto old = it++;
    CHECK(old->from == 0 && old->to == 1 && it->to == 2);
    ++it; ++it; ++it;
    CHECK((*it == G::Edge{3, 3}));
    auto prev = it--;
    CHECK(prev->from == 3 && it->from == 2);
    --it;
    CHECK(it->from == 1);
    G::const_edge_iterator cit = g.begin_edges();
    CHECK(cit == g.cbegin_edges());
    CHECK((*cit != G::Edge{0, 2}));
    auto e = g.end_edges();
    --e;  // с end() назад
    CHECK(e->from == 3);
}

TEST(graph_edge_iterators_skip_vertices_without_edges) {
    G g;
    auto a = g.add_vertex("a"), b = g.add_vertex("b"), c = g.add_vertex("c"), d = g.add_vertex("d");
    g.add_edge(b, c);
    (void)a; (void)d;
    CHECK(edges_of(g.begin_edges(), g.end_edges()) == (Pairs{{1, 2}}));
    CHECK(edges_of(g.rbegin_edges(), g.rend_edges()) == (Pairs{{1, 2}}));
    auto it = g.end_edges();
    --it;
    CHECK(it == g.begin_edges());
}

TEST(graph_incident_iterators) {
    G g = make();
    CHECK(edges_of(g.begin_incident(0), g.end_incident(0)) == (Pairs{{0, 1}, {0, 2}, {2, 0}}));
    CHECK(edges_of(g.cbegin_incident(0), g.cend_incident(0)) == (Pairs{{0, 1}, {0, 2}, {2, 0}}));
    CHECK(edges_of(g.rbegin_incident(0), g.rend_incident(0)) == (Pairs{{2, 0}, {0, 2}, {0, 1}}));
    CHECK(edges_of(g.crbegin_incident(0), g.crend_incident(0)) == (Pairs{{2, 0}, {0, 2}, {0, 1}}));
    CHECK(edges_of(g.begin_incident(3), g.end_incident(3)) == (Pairs{{3, 3}}));  // петля — одно ребро
    const G& cg = g;
    CHECK(edges_of(cg.begin_incident(1), cg.end_incident(1)) == (Pairs{{1, 2}, {0, 1}}));
    CHECK(edges_of(cg.rbegin_incident(1), cg.rend_incident(1)) == (Pairs{{0, 1}, {1, 2}}));
    auto it = g.begin_incident(1);
    auto old = it++;
    CHECK(old->to == 2 && it->from == 0);
    auto back = it--;
    CHECK(back->from == 0 && it->from == 1);
    --(++it);
    CHECK(it == g.begin_incident(1));
    G::const_incident_edge_iterator c = it;
    CHECK(c == cg.cbegin_incident(1));
    CHECK_THROWS(g.begin_incident(99), vertex_not_found);
}

TEST(graph_adjacent_iterators) {
    G g = make();
    CHECK(values_of(g.begin_adjacent(0), g.end_adjacent(0)) == "BC");
    CHECK(values_of(g.cbegin_adjacent(0), g.cend_adjacent(0)) == "BC");
    CHECK(values_of(g.rbegin_adjacent(0), g.rend_adjacent(0)) == "CB");
    CHECK(values_of(g.crbegin_adjacent(0), g.crend_adjacent(0)) == "CB");
    CHECK(values_of(g.begin_adjacent(3), g.end_adjacent(3)) == "D");
    CHECK(g.begin_adjacent(1) != g.end_adjacent(1));
    const G& cg = g;
    CHECK(values_of(cg.begin_adjacent(0), cg.end_adjacent(0)) == "BC");
    CHECK(values_of(cg.rbegin_adjacent(0), cg.rend_adjacent(0)) == "CB");
    auto it = g.begin_adjacent(0);
    it->value = "B2";  // изменение через неконстантный итератор
    CHECK(g.value(1) == "B2");
    auto old = it++;
    CHECK(old->id == 1 && it->id == 2);
    auto back = it--;
    CHECK(back->id == 2 && (*it).id == 1);
    --(++it);
    G::const_adjacent_vertex_iterator c = it;
    CHECK(c->id == 1);
    CHECK_THROWS(g.end_adjacent(99), vertex_not_found);
}

TEST(graph_erase_vertex_by_iterator) {
    G g = make();
    auto it = g.erase_vertex(g.find_vertex(1));
    CHECK(it->value == "C");  // следующая вершина
    CHECK(g.vertex_count() == 3 && g.edge_count() == 3);
    CHECK(!g.has_vertex(1));
    auto last = g.erase_vertex(std::prev(g.end_vertices()));
    CHECK(last == g.end_vertices());
    CHECK_THROWS(g.erase_vertex(g.end_vertices()), graph_error);
    // удаление всех вершин в цикле
    for (auto i = g.begin_vertices(); i != g.end_vertices();) i = g.erase_vertex(i);
    CHECK(g.empty() && g.edge_count() == 0);
}

TEST(graph_erase_edge_by_iterator) {
    G g = make();
    auto it = g.erase_edge(g.find_edge(0, 1));
    CHECK((*it == G::Edge{0, 2}));
    CHECK(g.edge_count() == 4);
    it = g.erase_edge(g.find_edge(2, 0));  // следующее — через границу вершины
    CHECK((*it == G::Edge{3, 3}));
    it = g.erase_edge(g.find_edge(3, 3));
    CHECK(it == g.end_edges());
    CHECK_THROWS(g.erase_edge(g.end_edges()), graph_error);
    CHECK(g.find_edge(0, 1) == g.end_edges());
    CHECK(g.find_edge(9, 0) == g.end_edges());
    CHECK(g.find_edge(0, 9) == g.end_edges());
    CHECK(g.find_edge(1, 1) == g.end_edges());
    CHECK(g.find_edge(0, 3) == g.end_edges());
    // удаление всех рёбер в цикле
    for (auto i = g.begin_edges(); i != g.end_edges();) i = g.erase_edge(i);
    CHECK(g.edge_count() == 0 && g.vertex_count() == 4);
}

TEST(graph_copy_constructor_and_assignment) {
    G a = make();
    G b(a);
    CHECK(a == b);
    b.add_vertex("E");
    CHECK(a != b);
    CHECK(a.vertex_count() == 4);       // копия независима
    b.value(0) = "changed";
    CHECK(a.value(0) == "A");
    G c;
    c = a;
    CHECK(c == a);
    c = c;  // самоприсваивание
    CHECK(c == a);
    G d(std::move(c));
    CHECK(d == a);
    G e;
    e = std::move(d);
    CHECK(e == a);
    CHECK(e.begin_vertices()->value == "A");  // итераторы после перемещения валидны
    G f;
    f.add_vertex("x");
    e.swap(f);
    CHECK(e.vertex_count() == 1 && f == a);
}

TEST(graph_comparison_operators) {
    G a = make(), b = make();
    CHECK(a == b);
    CHECK(!(a != b));
    CHECK(a <= b && a >= b && !(a < b) && !(a > b));
    b.add_vertex("E");
    CHECK(a < b && a <= b && b > a && b >= a && a != b);
    G c = make();
    c.remove_edge(3, 3);  // те же вершины, меньше рёбер
    CHECK(c < a && a > c);
    G d = make();
    d.value(0) = "0";  // "0" < "A"
    CHECK(d < a && !(a < d));
    G e = make();
    e.remove_edge(0, 1);
    e.add_edge(1, 0);  // те же вершины и число рёбер, другие рёбра
    CHECK(e != a && (e < a || a < e));
    // сравнение не зависит от конкретных id, только от порядка
    G x, y;
    x.add_vertex("p"); x.add_vertex("q"); x.add_edge(0, 1);
    y.add_vertex("tmp"); y.add_vertex("p"); y.add_vertex("q");
    y.remove_vertex(0);
    y.add_edge(1, 2);
    CHECK(x == y);
}

TEST(graph_output_uses_iterators) {
    G g;
    std::ostringstream os;
    os << g;
    CHECK(os.str() == "Graph{V=[], E=[]}");
    os.str("");
    os << make();
    CHECK(os.str() == "Graph{V=[0:A, 1:B, 2:C, 3:D], E=[0->1, 0->2, 1->2, 2->0, 3->3]}");
}

TEST(graph_generic_int_values) {
    DirectedGraph<int> g;
    auto a = g.add_vertex(10), b = g.add_vertex(20);
    g.add_edge(a, b);
    CHECK(g.value(b) == 20);
    DirectedGraph<int> h(g);
    CHECK(h == g);
    CHECK(!(h < g));
}
