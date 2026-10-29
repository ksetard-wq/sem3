// Минимальный тестовый фреймворк (один заголовок, без внешних зависимостей).
#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace mini {
struct Case { const char* name; std::function<void()> fn; };
inline std::vector<Case>& registry() { static std::vector<Case> r; return r; }
inline int& failures() { static int f = 0; return f; }
inline int& checks() { static int c = 0; return c; }
struct Registrar { Registrar(const char* n, std::function<void()> f) { registry().push_back({n, std::move(f)}); } };
inline int run_all() {
    for (auto& c : registry()) {
        int before = failures();
        try { c.fn(); } catch (const std::exception& e) { ++failures(); std::cout << "  EXCEPTION in " << c.name << ": " << e.what() << "\n"; }
        std::cout << (failures() == before ? "[ OK ] " : "[FAIL] ") << c.name << "\n";
    }
    std::cout << "\nТестов: " << registry().size() << ", проверок: " << checks() << ", провалов: " << failures() << "\n";
    return failures() == 0 ? 0 : 1;
}
}  // namespace mini

#define TEST(name) \
    static void name(); \
    static mini::Registrar reg_##name(#name, name); \
    static void name()

#define CHECK(cond) do { ++mini::checks(); if (!(cond)) { ++mini::failures(); \
    std::cout << "  CHECK failed: " #cond " (" << __FILE__ << ":" << __LINE__ << ")\n"; } } while (0)

#define CHECK_THROWS(expr, ExType) do { ++mini::checks(); bool ok_ = false; \
    try { expr; } catch (const ExType&) { ok_ = true; } catch (...) {} \
    if (!ok_) { ++mini::failures(); std::cout << "  CHECK_THROWS failed: " #expr " (" << __FILE__ << ":" << __LINE__ << ")\n"; } } while (0)
