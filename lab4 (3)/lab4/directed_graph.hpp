
#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace lab {

// ---------------------------- исключения ----------------------------
class graph_error : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
class vertex_not_found : public graph_error {
public:
    explicit vertex_not_found(std::size_t id) : graph_error("vertex not found: " + std::to_string(id)) {}
};
class edge_not_found : public graph_error {
public:
    edge_not_found(std::size_t a, std::size_t b)
        : graph_error("edge not found: " + std::to_string(a) + "->" + std::to_string(b)) {}
};
class duplicate_edge : public graph_error {
public:
    duplicate_edge(std::size_t a, std::size_t b)
        : graph_error("edge already exists: " + std::to_string(a) + "->" + std::to_string(b)) {}
};

template <class T>
class DirectedGraph {
public:
    // ------------------------ публичные типы ------------------------
    using value_type = T;
    using vertex_id = std::size_t;
    using size_type = std::size_t;

    // Вершина: неизменяемый id + изменяемое значение.
    struct Vertex {
        const vertex_id id;
        T value;
        Vertex(vertex_id i, T v) : id(i), value(std::move(v)) {}
    };

    // Ребро — пара (откуда, куда), аналог std::pair.
    struct Edge {
        vertex_id from;
        vertex_id to;
        friend bool operator==(const Edge& a, const Edge& b) { return a.from == b.from && a.to == b.to; }
        friend bool operator!=(const Edge& a, const Edge& b) { return !(a == b); }
    };

private:
    struct Slot {
        typename std::list<Vertex>::iterator it;
        std::vector<vertex_id> out;
        std::vector<vertex_id> in;
    };
    using SlotMap = std::map<vertex_id, Slot>;

    // ===================== итератор рёбер графа =====================
    // Проходит все рёбра: по возрастанию id начала, затем id конца.
    template <bool Const>
    class EdgeIter {
        using MapIt = std::conditional_t<Const, typename SlotMap::const_iterator, typename SlotMap::iterator>;
        template <bool>
        friend class EdgeIter;
        friend class DirectedGraph;

    public:
        struct Arrow {
            Edge e;
            const Edge* operator->() const { return &e; }
        };
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Edge;
        using difference_type = std::ptrdiff_t;
        using pointer = Arrow;
        using reference = Edge;  // ребро — пара значений, возвращается по значению

        EdgeIter() = default;
        template <bool C2, class = std::enable_if_t<Const && !C2>>
        EdgeIter(const EdgeIter<C2>& o) : outer_(o.outer_), end_(o.end_), idx_(o.idx_) {}

        reference operator*() const { return Edge{outer_->first, outer_->second.out[idx_]}; }
        pointer operator->() const { return Arrow{**this}; }

        EdgeIter& operator++() {
            ++idx_;
            normalize();
            return *this;
        }
        EdgeIter operator++(int) { EdgeIter t = *this; ++*this; return t; }
        EdgeIter& operator--() {
            if (outer_ != end_ && idx_ > 0) {
                --idx_;
                return *this;
            }
            do { --outer_; } while (outer_->second.out.empty());
            idx_ = outer_->second.out.size() - 1;
            return *this;
        }
        EdgeIter operator--(int) { EdgeIter t = *this; --*this; return t; }
        friend bool operator==(const EdgeIter& a, const EdgeIter& b) { return a.outer_ == b.outer_ && a.idx_ == b.idx_; }
        friend bool operator!=(const EdgeIter& a, const EdgeIter& b) { return !(a == b); }

    private:
        EdgeIter(MapIt outer, MapIt end, std::size_t idx) : outer_(outer), end_(end), idx_(idx) { normalize(); }
        void normalize() {  // пропускаем вершины, у которых кончился список исходящих
            while (outer_ != end_ && idx_ >= outer_->second.out.size()) {
                ++outer_;
                idx_ = 0;
            }
        }
        MapIt outer_{};
        MapIt end_{};
        std::size_t idx_ = 0;
    };

    // ============== итератор рёбер, инцидентных вершине ==============
    // Последовательность: исходящие рёбра, затем входящие.
    template <bool Const>
    class IncidentIter {
        template <bool>
        friend class IncidentIter;
        friend class DirectedGraph;

    public:
        struct Arrow {
            Edge e;
            const Edge* operator->() const { return &e; }
        };
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Edge;
        using difference_type = std::ptrdiff_t;
        using pointer = Arrow;
        using reference = Edge;

        IncidentIter() = default;
        template <bool C2, class = std::enable_if_t<Const && !C2>>
        IncidentIter(const IncidentIter<C2>& o) : slot_(o.slot_), v_(o.v_), pos_(o.pos_) {}

        reference operator*() const {
            const std::size_t n_out = slot_->out.size();
            return pos_ < n_out ? Edge{v_, slot_->out[pos_]} : Edge{slot_->in[pos_ - n_out], v_};
        }
        pointer operator->() const { return Arrow{**this}; }
        IncidentIter& operator++() { ++pos_; return *this; }
        IncidentIter operator++(int) { IncidentIter t = *this; ++pos_; return t; }
        IncidentIter& operator--() { --pos_; return *this; }
        IncidentIter operator--(int) { IncidentIter t = *this; --pos_; return t; }
        friend bool operator==(const IncidentIter& a, const IncidentIter& b) { return a.slot_ == b.slot_ && a.pos_ == b.pos_; }
        friend bool operator!=(const IncidentIter& a, const IncidentIter& b) { return !(a == b); }

    private:
        IncidentIter(const Slot* s, vertex_id v, std::size_t pos) : slot_(s), v_(v), pos_(pos) {}
        const Slot* slot_ = nullptr;
        vertex_id v_ = 0;
        std::size_t pos_ = 0;
    };

    // ============== итератор вершин, смежных вершине ==============
    // Разыменование даёт настоящую ссылку на вершину (можно менять value
    // через неконстантный итератор).
    template <bool Const>
    class AdjacentIter {
        using GraphPtr = std::conditional_t<Const, const DirectedGraph*, DirectedGraph*>;
        template <bool>
        friend class AdjacentIter;
        friend class DirectedGraph;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Vertex;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<Const, const Vertex*, Vertex*>;
        using reference = std::conditional_t<Const, const Vertex&, Vertex&>;

        AdjacentIter() = default;
        template <bool C2, class = std::enable_if_t<Const && !C2>>
        AdjacentIter(const AdjacentIter<C2>& o) : g_(o.g_), list_(o.list_), pos_(o.pos_) {}

        reference operator*() const { return *g_->slots_.at((*list_)[pos_]).it; }
        pointer operator->() const { return &**this; }
        AdjacentIter& operator++() { ++pos_; return *this; }
        AdjacentIter operator++(int) { AdjacentIter t = *this; ++pos_; return t; }
        AdjacentIter& operator--() { --pos_; return *this; }
        AdjacentIter operator--(int) { AdjacentIter t = *this; --pos_; return t; }
        friend bool operator==(const AdjacentIter& a, const AdjacentIter& b) { return a.list_ == b.list_ && a.pos_ == b.pos_; }
        friend bool operator!=(const AdjacentIter& a, const AdjacentIter& b) { return !(a == b); }

    private:
        AdjacentIter(GraphPtr g, const std::vector<vertex_id>* l, std::size_t pos) : g_(g), list_(l), pos_(pos) {}
        GraphPtr g_ = nullptr;
        const std::vector<vertex_id>* list_ = nullptr;
        std::size_t pos_ = 0;
    };

public:
    // ------------------------ типы итераторов ------------------------
    using vertex_iterator = typename std::list<Vertex>::iterator;
    using const_vertex_iterator = typename std::list<Vertex>::const_iterator;
    using reverse_vertex_iterator = std::reverse_iterator<vertex_iterator>;
    using const_reverse_vertex_iterator = std::reverse_iterator<const_vertex_iterator>;

    using edge_iterator = EdgeIter<false>;
    using const_edge_iterator = EdgeIter<true>;
    using reverse_edge_iterator = std::reverse_iterator<edge_iterator>;
    using const_reverse_edge_iterator = std::reverse_iterator<const_edge_iterator>;

    using incident_edge_iterator = IncidentIter<false>;
    using const_incident_edge_iterator = IncidentIter<true>;
    using reverse_incident_edge_iterator = std::reverse_iterator<incident_edge_iterator>;
    using const_reverse_incident_edge_iterator = std::reverse_iterator<const_incident_edge_iterator>;

    using adjacent_vertex_iterator = AdjacentIter<false>;
    using const_adjacent_vertex_iterator = AdjacentIter<true>;
    using reverse_adjacent_vertex_iterator = std::reverse_iterator<adjacent_vertex_iterator>;
    using const_reverse_adjacent_vertex_iterator = std::reverse_iterator<const_adjacent_vertex_iterator>;

    // ------------------------ конструкторы ------------------------
    DirectedGraph() = default;
    DirectedGraph(const DirectedGraph& other) { copy_from(other); }
    DirectedGraph(DirectedGraph&&) noexcept = default;  // итераторы list остаются валидными
    DirectedGraph& operator=(DirectedGraph&&) noexcept = default;
    DirectedGraph& operator=(const DirectedGraph& other) {  // copy-and-swap: строгая гарантия
        if (this != &other) {
            DirectedGraph tmp(other);
            swap(tmp);
        }
        return *this;
    }
    ~DirectedGraph() = default;  // всё освобождается автоматически (RAII: list/map/vector)

    void swap(DirectedGraph& o) noexcept {
        vertices_.swap(o.vertices_);
        slots_.swap(o.slots_);
        std::swap(edge_count_, o.edge_count_);
        std::swap(next_id_, o.next_id_);
    }

    // ------------------------ состояние ------------------------
    bool empty() const { return vertices_.empty(); }
    void clear() {
        vertices_.clear();
        slots_.clear();
        edge_count_ = 0;  // next_id_ не сбрасываем: id не переиспользуются
    }
    size_type vertex_count() const { return vertices_.size(); }
    size_type edge_count() const { return edge_count_; }

    // ------------------------ проверки ------------------------
    bool has_vertex(vertex_id id) const { return slots_.find(id) != slots_.end(); }
    bool has_edge(vertex_id from, vertex_id to) const {
        auto f = slots_.find(from);
        return f != slots_.end() && has_vertex(to) && contains(f->second.out, to);
    }

    // ------------------------ доступ ------------------------
    T& value(vertex_id id) { return slot(id).it->value; }
    const T& value(vertex_id id) const { return slot(id).it->value; }

    size_type out_degree(vertex_id id) const { return slot(id).out.size(); }
    size_type in_degree(vertex_id id) const {
        const Slot& s = slot(id);
        return s.in.size() + (contains(s.out, id) ? 1 : 0);  // петля — и вход, и выход
    }
    size_type degree(vertex_id id) const { return out_degree(id) + in_degree(id); }

    // Степень ребра: сколько других рёбер имеют общий конец с (from,to).
    size_type edge_degree(vertex_id from, vertex_id to) const {
        if (!has_edge(from, to)) {
            if (!has_vertex(from)) throw vertex_not_found(from);
            if (!has_vertex(to)) throw vertex_not_found(to);
            throw edge_not_found(from, to);
        }
        std::set<std::pair<vertex_id, vertex_id>> touching;
        for (vertex_id x : {from, to}) {
            const Slot& s = slots_.at(x);
            for (vertex_id y : s.out) touching.insert({x, y});
            for (vertex_id y : s.in) touching.insert({y, x});
        }
        return touching.size() - 1;  // минус само ребро
    }

    // ------------------------ добавление ------------------------
    vertex_id add_vertex(T value) {
        const vertex_id id = next_id_++;
        vertices_.emplace_back(id, std::move(value));
        slots_[id] = Slot{std::prev(vertices_.end()), {}, {}};
        return id;
    }

    void add_edge(vertex_id from, vertex_id to) {
        Slot& a = slot(from);
        Slot& b = slot(to);
        if (contains(a.out, to)) throw duplicate_edge(from, to);
        insert_sorted(a.out, to);
        if (from != to) insert_sorted(b.in, from);
        ++edge_count_;
    }

    // ------------------------ удаление ------------------------
    void remove_edge(vertex_id from, vertex_id to) {
        Slot& a = slot(from);
        Slot& b = slot(to);
        if (!contains(a.out, to)) throw edge_not_found(from, to);
        erase_sorted(a.out, to);
        if (from != to) erase_sorted(b.in, from);
        --edge_count_;
    }

    void remove_vertex(vertex_id id) { erase_vertex(const_vertex_iterator(slot(id).it)); }

    // Удаление по итератору. Возвращают итератор на следующий элемент.
    vertex_iterator erase_vertex(const_vertex_iterator pos) {
        if (pos == vertices_.cend()) throw graph_error("cannot erase end() iterator");
        const vertex_id id = pos->id;
        detach_edges(id);
        slots_.erase(id);
        return vertices_.erase(pos);
    }

    edge_iterator erase_edge(const_edge_iterator pos) {
        if (pos == cend_edges()) throw graph_error("cannot erase end() iterator");
        const Edge e = *pos;
        remove_edge(e.from, e.to);
        auto outer = slots_.find(e.from);
        const auto& out = outer->second.out;
        const std::size_t idx = static_cast<std::size_t>(std::lower_bound(out.begin(), out.end(), e.to) - out.begin());
        return edge_iterator(outer, slots_.end(), idx);
    }

    // ------------------------ поиск ------------------------
    vertex_iterator find_vertex(vertex_id id) {
        auto f = slots_.find(id);
        return f == slots_.end() ? vertices_.end() : f->second.it;
    }
    const_vertex_iterator find_vertex(vertex_id id) const {
        auto f = slots_.find(id);
        return f == slots_.end() ? vertices_.cend() : const_vertex_iterator(f->second.it);
    }
    edge_iterator find_edge(vertex_id from, vertex_id to) {
        auto f = slots_.find(from);
        if (f == slots_.end() || !has_vertex(to)) return end_edges();
        const auto& out = f->second.out;
        auto p = std::lower_bound(out.begin(), out.end(), to);
        if (p == out.end() || *p != to) return end_edges();
        return edge_iterator(f, slots_.end(), static_cast<std::size_t>(p - out.begin()));
    }

    // ------------------------ итераторы вершин ------------------------
    vertex_iterator begin_vertices() { return vertices_.begin(); }
    vertex_iterator end_vertices() { return vertices_.end(); }
    const_vertex_iterator begin_vertices() const { return vertices_.begin(); }
    const_vertex_iterator end_vertices() const { return vertices_.end(); }
    const_vertex_iterator cbegin_vertices() const { return vertices_.cbegin(); }
    const_vertex_iterator cend_vertices() const { return vertices_.cend(); }
    reverse_vertex_iterator rbegin_vertices() { return reverse_vertex_iterator(end_vertices()); }
    reverse_vertex_iterator rend_vertices() { return reverse_vertex_iterator(begin_vertices()); }
    const_reverse_vertex_iterator rbegin_vertices() const { return const_reverse_vertex_iterator(end_vertices()); }
    const_reverse_vertex_iterator rend_vertices() const { return const_reverse_vertex_iterator(begin_vertices()); }
    const_reverse_vertex_iterator crbegin_vertices() const { return rbegin_vertices(); }
    const_reverse_vertex_iterator crend_vertices() const { return rend_vertices(); }

    // ------------------------ итераторы рёбер ------------------------
    edge_iterator begin_edges() { return edge_iterator(slots_.begin(), slots_.end(), 0); }
    edge_iterator end_edges() { return edge_iterator(slots_.end(), slots_.end(), 0); }
    const_edge_iterator begin_edges() const { return const_edge_iterator(slots_.begin(), slots_.end(), 0); }
    const_edge_iterator end_edges() const { return const_edge_iterator(slots_.end(), slots_.end(), 0); }
    const_edge_iterator cbegin_edges() const { return begin_edges(); }
    const_edge_iterator cend_edges() const { return end_edges(); }
    reverse_edge_iterator rbegin_edges() { return reverse_edge_iterator(end_edges()); }
    reverse_edge_iterator rend_edges() { return reverse_edge_iterator(begin_edges()); }
    const_reverse_edge_iterator rbegin_edges() const { return const_reverse_edge_iterator(end_edges()); }
    const_reverse_edge_iterator rend_edges() const { return const_reverse_edge_iterator(begin_edges()); }
    const_reverse_edge_iterator crbegin_edges() const { return rbegin_edges(); }
    const_reverse_edge_iterator crend_edges() const { return rend_edges(); }

    // ------------------------ итераторы рёбер, инцидентных вершине ------------------------
    incident_edge_iterator begin_incident(vertex_id v) { return incident_edge_iterator(&slot(v), v, 0); }
    incident_edge_iterator end_incident(vertex_id v) {
        const Slot& s = slot(v);
        return incident_edge_iterator(&s, v, s.out.size() + s.in.size());
    }
    const_incident_edge_iterator begin_incident(vertex_id v) const { return const_incident_edge_iterator(&slot(v), v, 0); }
    const_incident_edge_iterator end_incident(vertex_id v) const {
        const Slot& s = slot(v);
        return const_incident_edge_iterator(&s, v, s.out.size() + s.in.size());
    }
    const_incident_edge_iterator cbegin_incident(vertex_id v) const { return begin_incident(v); }
    const_incident_edge_iterator cend_incident(vertex_id v) const { return end_incident(v); }
    reverse_incident_edge_iterator rbegin_incident(vertex_id v) { return reverse_incident_edge_iterator(end_incident(v)); }
    reverse_incident_edge_iterator rend_incident(vertex_id v) { return reverse_incident_edge_iterator(begin_incident(v)); }
    const_reverse_incident_edge_iterator rbegin_incident(vertex_id v) const { return const_reverse_incident_edge_iterator(end_incident(v)); }
    const_reverse_incident_edge_iterator rend_incident(vertex_id v) const { return const_reverse_incident_edge_iterator(begin_incident(v)); }
    const_reverse_incident_edge_iterator crbegin_incident(vertex_id v) const { return rbegin_incident(v); }
    const_reverse_incident_edge_iterator crend_incident(vertex_id v) const { return rend_incident(v); }

    // ------------------------ итераторы смежных вершин ------------------------
    adjacent_vertex_iterator begin_adjacent(vertex_id v) { return adjacent_vertex_iterator(this, &slot(v).out, 0); }
    adjacent_vertex_iterator end_adjacent(vertex_id v) {
        const Slot& s = slot(v);
        return adjacent_vertex_iterator(this, &s.out, s.out.size());
    }
    const_adjacent_vertex_iterator begin_adjacent(vertex_id v) const { return const_adjacent_vertex_iterator(this, &slot(v).out, 0); }
    const_adjacent_vertex_iterator end_adjacent(vertex_id v) const {
        const Slot& s = slot(v);
        return const_adjacent_vertex_iterator(this, &s.out, s.out.size());
    }
    const_adjacent_vertex_iterator cbegin_adjacent(vertex_id v) const { return begin_adjacent(v); }
    const_adjacent_vertex_iterator cend_adjacent(vertex_id v) const { return end_adjacent(v); }
    reverse_adjacent_vertex_iterator rbegin_adjacent(vertex_id v) { return reverse_adjacent_vertex_iterator(end_adjacent(v)); }
    reverse_adjacent_vertex_iterator rend_adjacent(vertex_id v) { return reverse_adjacent_vertex_iterator(begin_adjacent(v)); }
    const_reverse_adjacent_vertex_iterator rbegin_adjacent(vertex_id v) const { return const_reverse_adjacent_vertex_iterator(end_adjacent(v)); }
    const_reverse_adjacent_vertex_iterator rend_adjacent(vertex_id v) const { return const_reverse_adjacent_vertex_iterator(begin_adjacent(v)); }
    const_reverse_adjacent_vertex_iterator crbegin_adjacent(vertex_id v) const { return rbegin_adjacent(v); }
    const_reverse_adjacent_vertex_iterator crend_adjacent(vertex_id v) const { return rend_adjacent(v); }

    // ------------------------ сравнение ------------------------
    // Графы равны, если совпадают последовательности значений вершин
    // (в порядке добавления) и множества рёбер между вершинами с одинаковыми
    // порядковыми номерами. Порядок < : сначала значения лексикографически,
    // затем список рёбер. Для < и > нужен operator< у T.
    friend bool operator==(const DirectedGraph& a, const DirectedGraph& b) {
        return a.vertices_.size() == b.vertices_.size() && a.edge_count_ == b.edge_count_ &&
               std::equal(a.vertices_.begin(), a.vertices_.end(), b.vertices_.begin(),
                          [](const Vertex& x, const Vertex& y) { return x.value == y.value; }) &&
               a.index_edges() == b.index_edges();
    }
    friend bool operator!=(const DirectedGraph& a, const DirectedGraph& b) { return !(a == b); }
    friend bool operator<(const DirectedGraph& a, const DirectedGraph& b) {
        auto less = [](const Vertex& x, const Vertex& y) { return x.value < y.value; };
        if (std::lexicographical_compare(a.vertices_.begin(), a.vertices_.end(), b.vertices_.begin(), b.vertices_.end(), less)) return true;
        if (std::lexicographical_compare(b.vertices_.begin(), b.vertices_.end(), a.vertices_.begin(), a.vertices_.end(), less)) return false;
        return a.index_edges() < b.index_edges();
    }
    friend bool operator>(const DirectedGraph& a, const DirectedGraph& b) { return b < a; }
    friend bool operator<=(const DirectedGraph& a, const DirectedGraph& b) { return !(b < a); }
    friend bool operator>=(const DirectedGraph& a, const DirectedGraph& b) { return !(a < b); }

    // ------------------------ вывод ------------------------
    // Текст строится через итераторы и обобщённый алгоритм std::for_each.
    friend std::ostream& operator<<(std::ostream& os, const DirectedGraph& g) {
        bool first = true;
        os << "Graph{V=[";
        std::for_each(g.cbegin_vertices(), g.cend_vertices(), [&](const Vertex& v) {
            os << (first ? "" : ", ") << v.id << ":" << v.value;
            first = false;
        });
        first = true;
        os << "], E=[";
        std::for_each(g.cbegin_edges(), g.cend_edges(), [&](const Edge& e) {
            os << (first ? "" : ", ") << e.from << "->" << e.to;
            first = false;
        });
        return os << "]}";
    }

private:
    // ------------------------ служебное ------------------------
    Slot& slot(vertex_id id) {
        auto f = slots_.find(id);
        if (f == slots_.end()) throw vertex_not_found(id);
        return f->second;
    }
    const Slot& slot(vertex_id id) const {
        auto f = slots_.find(id);
        if (f == slots_.end()) throw vertex_not_found(id);
        return f->second;
    }
    static bool contains(const std::vector<vertex_id>& v, vertex_id x) { return std::binary_search(v.begin(), v.end(), x); }
    static void insert_sorted(std::vector<vertex_id>& v, vertex_id x) { v.insert(std::lower_bound(v.begin(), v.end(), x), x); }
    static void erase_sorted(std::vector<vertex_id>& v, vertex_id x) { v.erase(std::lower_bound(v.begin(), v.end(), x)); }

    // Удаляет все рёбра вершины (но не саму вершину).
    void detach_edges(vertex_id id) {
        Slot& s = slot(id);
        for (vertex_id t : s.out)
            if (t != id) erase_sorted(slot(t).in, id);
        for (vertex_id f : s.in) erase_sorted(slot(f).out, id);
        edge_count_ -= s.out.size() + s.in.size();
        s.out.clear();
        s.in.clear();
    }

    // Рёбра как пары порядковых номеров вершин (для сравнения графов).
    std::vector<std::pair<size_type, size_type>> index_edges() const {
        std::map<vertex_id, size_type> pos;
        size_type i = 0;
        for (const Vertex& v : vertices_) pos[v.id] = i++;
        std::vector<std::pair<size_type, size_type>> res;
        for (auto it = cbegin_edges(); it != cend_edges(); ++it) {
            const Edge e = *it;
            res.emplace_back(pos[e.from], pos[e.to]);
        }
        return res;
    }

    void copy_from(const DirectedGraph& o) {
        for (const Vertex& v : o.vertices_) {
            vertices_.emplace_back(v.id, v.value);
            const Slot& src = o.slots_.at(v.id);
            slots_[v.id] = Slot{std::prev(vertices_.end()), src.out, src.in};
        }
        edge_count_ = o.edge_count_;
        next_id_ = o.next_id_;
    }

    std::list<Vertex> vertices_;
    SlotMap slots_;
    size_type edge_count_ = 0;
    vertex_id next_id_ = 0;
};

}  // namespace lab
