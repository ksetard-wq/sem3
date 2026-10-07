#include <gtest/gtest.h>
#include <cstring>
#include "Multiset.h"

// ==========================================
// 1. ТЕСТЫ КЛАССА ATOM (Тесты 1-6)
// ==========================================

TEST(AtomTest, ConstructorAndGet) {
    Atom a("test");
    EXPECT_STREQ(a.get(), "test");
}

TEST(AtomTest, EqualsSame) {
    Atom a1("hello");
    Atom a2("hello");
    EXPECT_TRUE(a1.equals(&a2));
}

TEST(AtomTest, EqualsDifferent) {
    Atom a1("hello");
    Atom a2("world");
    EXPECT_FALSE(a1.equals(&a2));
}

TEST(AtomTest, Clone) {
    Atom a1("original");
    Element* cloned = a1.clone();
    EXPECT_TRUE(a1.equals(cloned));
    delete cloned;
}

TEST(AtomTest, BufferOverflowTruncation) {
    Atom a("1234567890123456789012345678901234567890");
    EXPECT_EQ(std::strlen(a.get()), 31u);
}

TEST(AtomTest, EmptyAtom) {
    Atom a("");
    EXPECT_STREQ(a.get(), "");
}

// ==========================================
// 2. ТЕСТЫ КЛАССА MULTISET (Тесты 7-23)
// ==========================================

TEST(MultisetTest, DefaultConstructor) {
    Multiset m;
    EXPECT_EQ(m.size(), 0u);
}

TEST(MultisetTest, AddAndSize) {
    Multiset m;
    Atom a1("a"), a2("b");
    m.add(&a1);
    m.add(&a2);
    EXPECT_EQ(m.size(), 2u);
}

TEST(MultisetTest, AddDuplicateElements) {
    Multiset m;
    Atom a("dup");
    m.add(&a);
    m.add(&a);
    EXPECT_EQ(m.size(), 2u);
}

TEST(MultisetTest, RemoveExisting) {
    Multiset m;
    Atom a1("a"), a2("b");
    m.add(&a1);
    m.add(&a2);
    EXPECT_TRUE(m.remove(&a1));
    EXPECT_EQ(m.size(), 1u);
}

TEST(MultisetTest, RemoveNonExisting) {
    Multiset m;
    Atom a1("a"), a2("b");
    m.add(&a1);
    EXPECT_FALSE(m.remove(&a2));
    EXPECT_EQ(m.size(), 1u);
}

TEST(MultisetTest, Clear) {
    Multiset m;
    Atom a("x");
    m.add(&a);
    m.clear();
    EXPECT_EQ(m.size(), 0u);
}

TEST(MultisetTest, CopyConstructor) {
    Multiset m1;
    Atom a("x");
    m1.add(&a);
    Multiset m2(m1);
    EXPECT_TRUE(m1.equals(&m2));
    EXPECT_EQ(m2.size(), 1u);
}

TEST(MultisetTest, AssignmentOperator) {
    Multiset m1, m2;
    Atom a("y");
    m1.add(&a);
    m2 = m1;
    EXPECT_TRUE(m1.equals(&m2));
}

TEST(MultisetTest, SelfAssignment) {
    Multiset m;
    Atom a("z");
    m.add(&a);
    m = m;
    EXPECT_EQ(m.size(), 1u);
}

TEST(MultisetTest, NestedMultisetAdd) {
    Multiset parent, child;
    Atom a("inner");
    child.add(&a);
    parent.add(&child);
    EXPECT_EQ(parent.size(), 1u);
}

TEST(MultisetTest, EqualsIdentical) {
    Multiset m1, m2;
    Atom a1("a"), a2("a");
    m1.add(&a1);
    m2.add(&a2);
    EXPECT_TRUE(m1.equals(&m2));
}

TEST(MultisetTest, EqualsDifferentSize) {
    Multiset m1, m2;
    Atom a1("a");
    m1.add(&a1);
    EXPECT_FALSE(m1.equals(&m2));
}

TEST(MultisetTest, OperatorPlusAssign) {
    Multiset m1, m2;
    Atom a1("a"), a2("b");
    m1.add(&a1);
    m2.add(&a2);
    m1 += m2;
    EXPECT_EQ(m1.size(), 2u);
}

TEST(MultisetTest, OperatorMinusAssign) {
    Multiset m1, m2;
    Atom a1("a"), a2("b");
    m1.add(&a1);
    m1.add(&a2);
    m2.add(&a1);
    m1 -= m2;
    EXPECT_EQ(m1.size(), 1u);
}

TEST(MultisetTest, BinaryOperatorPlus) {
    Multiset m1, m2;
    Atom a1("a"), a2("b");
    m1.add(&a1);
    m2.add(&a2);
    Multiset res = m1 + m2;
    EXPECT_EQ(res.size(), 2u);
}

TEST(MultisetTest, BinaryOperatorMinus) {
    Multiset m1, m2;
    Atom a1("a"), a2("b");
    m1.add(&a1);
    m1.add(&a2);
    m2.add(&a1);
    Multiset res = m1 - m2;
    EXPECT_EQ(res.size(), 1u);
}

TEST(MultisetTest, CloneMethod) {
    Multiset m;
    Atom a("val");
    m.add(&a);
    Element* cloned = m.clone();
    EXPECT_TRUE(m.equals(cloned));
    delete cloned;
}

// ==========================================
// 3. ТЕСТЫ ПАРСЕРА MULTISET (Тесты 24-30)
// ==========================================

TEST(ParserTest, ParseEmpty) {
    const char* str = "{}";
    Multiset m = Multiset::parse(str);
    EXPECT_EQ(m.size(), 0u);
}

TEST(ParserTest, ParseAtoms) {
    const char* str = "{a, b, c}";
    Multiset m = Multiset::parse(str);
    EXPECT_EQ(m.size(), 3u);
}

TEST(ParserTest, ParseNested) {
    const char* str = "{a, {b, c}}";
    Multiset m = Multiset::parse(str);
    EXPECT_EQ(m.size(), 2u);
}

TEST(ParserTest, ParseWithSpaces) {
    const char* str = "{  x ,  y  }";
    Multiset m = Multiset::parse(str);
    EXPECT_EQ(m.size(), 2u);
}

TEST(ParserTest, ParseDeeplyNested) {
    const char* str = "{{{1}}}";
    Multiset m = Multiset::parse(str);
    EXPECT_EQ(m.size(), 1u);
}

TEST(ParserTest, ParseMultipleAtomsWithSameName) {
    const char* str = "{a, a, a}";
    Multiset m = Multiset::parse(str);
    EXPECT_EQ(m.size(), 3u);
}

TEST(ParserTest, ParseComplexStructure) {
    const char* str = "{a, {b}, {c, d}}";
    Multiset m = Multiset::parse(str);
    EXPECT_EQ(m.size(), 3u);
}