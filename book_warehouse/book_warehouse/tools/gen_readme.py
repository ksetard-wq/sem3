#!/usr/bin/env python3
"""Генерирует README.md по заголовкам из include/*.h.

Правила подсчёта:
  * класс — любой `class X {...}` (enum class не считается);
  * поле — член класса без скобок, имя заканчивается на `_`;
  * поведение — метод, который не является конструктором, деструктором,
    геттером (`getXxx`) или `what()`;
  * ассоциация — другой класс предметной области, встречающийся как тип поля,
    тип параметра метода или как базовый класс;
  * исключения — все классы из include/exceptions.h.
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INC = os.path.join(ROOT, "include")

DESCRIPTIONS = {
    "Accountant": "Бухгалтер: оплата счетов, ведение кассы, расчёт зарплат.",
    "ActivityLog": "Журнал действий системы с ограничением размера.",
    "Address": "Почтовый адрес.",
    "AuditDiscrepancy": "Расхождение между учётным и фактическим количеством книги.",
    "Author": "Автор книг.",
    "BarcodeScanner": "Сканер штрихкодов (ISBN-13) с зарядом батареи.",
    "Book": "Книга: ISBN, автор, издательство, жанр, цена, вес.",
    "BookCatalog": "Каталог книг: добавление, удаление, поиск.",
    "ClimateControl": "Климат-контроль зоны: температура и влажность.",
    "ContactInfo": "Телефон и e-mail с проверкой формата.",
    "Customer": "Клиент магазина (наследник Person).",
    "CustomerOrder": "Заказ клиента со статусами, скидкой и оплатой.",
    "Date": "Календарная дата с арифметикой.",
    "Discount": "Промокод со сроком действия и лимитом использований.",
    "Employee": "Сотрудник склада (наследник Person).",
    "Forklift": "Погрузчик: оператор, грузоподъёмность, заряд.",
    "Genre": "Жанр с возрастным ограничением.",
    "InboundShipment": "Входящая поставка: приёмка по закупке, док, инспекция.",
    "Inventory": "Складской учёт: остатки, движения, списания.",
    "InventoryAudit": "Инвентаризация и фиксация расхождений.",
    "ISBN": "Код ISBN-13 с проверкой контрольной цифры.",
    "LoadingDock": "Погрузочный док.",
    "Loader": "Грузчик (наследник Employee).",
    "LoyaltyCard": "Карта лояльности с баллами и уровнями.",
    "Money": "Денежная сумма в копейках с валютой.",
    "OrderLine": "Позиция заказа: книга, количество, цена.",
    "OutboundShipment": "Исходящая отправка заказа перевозчиком.",
    "Package": "Посылка клиенту.",
    "Pallet": "Паллета с ограничением по весу.",
    "Payment": "Платёж с жизненным циклом статусов.",
    "PayrollRecord": "Расчётный листок сотрудника.",
    "Person": "Базовый класс человека.",
    "Picker": "Комплектовщик заказов (наследник Employee).",
    "PricingPolicy": "Политика ценообразования: наценка, маржа, скидки.",
    "ProcurementManager": "Менеджер по закупкам (наследник Employee).",
    "Publisher": "Издательство.",
    "PurchaseOrder": "Заказ поставщику.",
    "ReportGenerator": "Текстовые отчёты по складу, продажам, зарплатам.",
    "Reservation": "Резерв книги за клиентом на срок.",
    "ReturnRequest": "Заявка на возврат книги.",
    "Schedule": "График смен сотрудников.",
    "Shelf": "Стеллаж с ограничением по весу.",
    "Shift": "Рабочая смена сотрудника.",
    "ShippingCarrier": "Перевозчик с тарифами.",
    "StockItem": "Складская позиция: количество, резерв, минимальный остаток.",
    "StockMovement": "Движение товара: приход, расход, списание.",
    "Storekeeper": "Кладовщик (наследник Employee), работает в своей зоне.",
    "Supplier": "Поставщик с прайс-листом и рейтингом.",
    "SupplierInvoice": "Счёт поставщика с расчётом пени.",
    "TaxCalculator": "НДС и подоходный налог.",
    "Warehouse": "Склад: зоны, доки, инвентарь.",
    "WarehouseManager": "Заведующий складом (наследник Employee).",
    "WarehouseSystem": "Фасад: связывает каталог, склад, клиентов, заказы, закупки.",
    "WriteOffRecord": "Запись о списании книг.",
    "Zone": "Зона склада со стеллажами и климатом.",
}


def strip_comments(text):
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            j = i + 1
            while j < n and text[j] != '"':
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
        elif text.startswith("//", i):
            while i < n and text[i] != "\n":
                i += 1
        elif text.startswith("/*", i):
            i = text.index("*/", i) + 2
        else:
            out.append(c)
            i += 1
    return "".join(out)


def match_brace(text, start):
    """text[start] == '{'; возвращает индекс парной '}' (строки пропускаются)."""
    depth, i, n = 0, start, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            i += 1
            while text[i] != '"':
                i += 2 if text[i] == "\\" else 1
        elif c == "'":
            i += 1
            while text[i] != "'":
                i += 2 if text[i] == "\\" else 1
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise ValueError("несбалансированные скобки")


def match_paren(text, start):
    depth = 0
    for i in range(start, len(text)):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                return i
    raise ValueError("несбалансированные скобки")


def split_members(body):
    members, i, n, cur = [], 0, len(body), []
    while i < n:
        c = body[i]
        if c == "{":
            j = match_brace(body, i)
            members.append("".join(cur))
            cur = []
            i = j + 1
            while i < n and body[i] in " \t\r\n;":
                i += 1
            continue
        if c == ";":
            members.append("".join(cur))
            cur = []
        else:
            cur.append(c)
        i += 1
    return [m.strip() for m in members if m.strip()]


def parse_file(path):
    text = strip_comments(open(path, encoding="utf-8").read())
    classes = []
    for m in re.finditer(r"\bclass\s+(\w+)\s*(?::\s*public\s+(\w+))?\s*\{", text):
        if text[:m.start()].rstrip().endswith("enum"):
            continue
        end = match_brace(text, m.end() - 1)
        body = text[m.end():end]
        cls = {"name": m.group(1), "base": m.group(2), "fields": [], "methods": [], "param_types": []}
        for stmt in split_members(body):
            stmt = re.sub(r"^(public|private|protected)\s*:\s*", "", stmt).strip()
            if not stmt or stmt.startswith(("using", "friend", "static_assert")):
                continue
            if "(" in stmt:
                head = stmt[:stmt.index("(")]
                name_m = re.search(r"(~?\w+)\s*$", head)
                if not name_m:
                    continue
                name = name_m.group(1)
                params = stmt[stmt.index("(") + 1:match_paren(stmt, stmt.index("("))]
                cls["methods"].append((name, " ".join(params.split())))
            else:
                decl = stmt.split("=")[0].strip()
                name_m = re.search(r"(\w+_)\s*$", decl)
                if name_m:
                    ftype = decl[:name_m.start()].strip()
                    cls["fields"].append((name_m.group(1), ftype))
        classes.append(cls)
    return classes


def is_behavior(cls, name):
    if name == cls["name"] or name.startswith("~") or name == "what":
        return False
    return not re.match(r"get[A-Z]", name)


def main():
    exceptions, domain = [], []
    for fname in sorted(os.listdir(INC)):
        if not fname.endswith(".h"):
            continue
        for cls in parse_file(os.path.join(INC, fname)):
            (exceptions if fname == "exceptions.h" else domain).append(cls)
    names = {c["name"] for c in domain}
    domain.sort(key=lambda c: c["name"])

    rows, total_f, total_b, assoc_pairs = [], 0, 0, 0
    for c in domain:
        behaviors = sorted({n for n, _ in c["methods"] if is_behavior(c, n)})
        c["behaviors"] = behaviors
        related = set()
        texts = [t for _, t in c["fields"]] + [p for _, p in c["methods"]]
        for t in texts:
            related |= set(re.findall(r"\b\w+\b", t)) & names
        if c["base"] in names:
            related.add(c["base"])
        related.discard(c["name"])
        c["related"] = sorted(related)
        total_f += len(c["fields"])
        total_b += len(behaviors)
        assoc_pairs += len(related)
        rows.append(f"| {c['name']} | {len(c['fields'])} | {len(behaviors)} | "
                    f"{', '.join(c['related']) if related else '—'} |")

    exc_fields = sum(len(c["fields"]) for c in exceptions)
    out = []
    out.append("# Лабораторная работа №2. Крупный ООП-проект\n")
    out.append("Предметная область: **книжный склад**. Модель описывает каталог книг (авторы, издательства, жанры), "
               "складскую структуру (зоны, стеллажи, паллеты, погрузочные доки, климат-контроль), складской учёт "
               "и инвентаризацию, закупки у поставщиков, заказы клиентов, оплату, доставку и возвраты, "
               "а также персонал (заведующий, кладовщики, комплектовщик, грузчик, бухгалтер, менеджер по закупкам), "
               "графики смен и зарплаты.\n")
    out.append("Язык: **C++17**. Все классы лежат в заголовочных файлах `include/*.h` (по одному классу на файл), "
               "консольное приложение и сценарии работы с классами — в `src/main.cpp`.\n")
    out.append("## Структура репозитория\n")
    out.append("```\ninclude/      классы предметной области (.h) и exceptions.h\nsrc/main.cpp  консольное меню и демо-сценарии\n"
               "tests/        модульные тесты (tests/tests.cpp)\ntools/        gen_readme.py — генератор этого README\n"
               "CMakeLists.txt\n```\n")
    out.append("## Сборка и запуск\n")
    out.append("```bash\ncmake -S . -B build\ncmake --build build\n./build/book_warehouse          # интерактивное меню\n"
               "./build/book_warehouse --demo   # автоматический демо-сценарий\n./build/run_tests              # тесты\n```\n")
    out.append("Без CMake: `g++ -std=c++17 -Iinclude src/main.cpp -o book_warehouse`. "
               "В Windows консоль переключается в UTF-8 автоматически.\n")
    out.append("## Консольное меню\n")
    out.append("Каталог и поиск книг; остатки, штрихкод, списание, погрузка паллет; закупка у поставщика; "
               "оформление заказа клиента (с промокодом и проверкой возрастного ограничения); оплата и отгрузка; "
               "подтверждение доставки; возврат; резервы; персонал, смены и зарплата; отчёты; климат в зонах; "
               "инвентаризация; журнал действий; демо-сценарий целиком; демонстрация всех исключений.\n")
    out.append("## Классы\n")
    out.append("Формат строки: `Класс | число полей | число методов (поведений) | связанные классы`. "
               "Геттеры, конструкторы и деструкторы в число методов не входят.\n")
    out.append("| Класс | Поля | Методы | Ассоциации (связанные классы) |")
    out.append("| ----- | ---- | ------ | ----------------------------- |")
    out.extend(rows)
    out.append(f"| Exceptions | {exc_fields} | 0 | " + ", ".join(c["name"] for c in exceptions[:3]) + " и др. |")
    out.append("")
    out.append(f"### Исключения ({len(exceptions)})\n")
    for c in exceptions:
        out.append(f"- {c['name']}" + (f" (наследник {c['base']})" if c["base"] and c["base"] != "std::runtime_error" else ""))
    out.append("")
    out.append("### Итоговая статистика\n")
    out.append("| Показатель | Значение |")
    out.append("| ---------- | -------- |")
    out.append(f"| Классы предметной области | {len(domain)} |")
    out.append(f"| Классы исключений | {len(exceptions)} |")
    out.append(f"| Поля | {total_f} |")
    out.append(f"| Поведения | {total_b} |")
    out.append(f"| Ассоциации | {assoc_pairs} |")
    out.append(f"| Исключения | {len(exceptions)} |")
    out.append("")
    out.append("Требования задания: ≥ 50 классов, ≥ 150 полей, ≥ 100 поведений, ≥ 30 ассоциаций, ≥ 12 исключений. "
               "Поля исключений (3 шт.) в общее число не входят. Поведения подсчитаны построчно по каждому классу, "
               "перегруженные методы с одним именем считаются один раз.\n")
    out.append("## Подробное описание классов\n")
    for c in domain:
        out.append(f"### {c['name']}\n")
        out.append(DESCRIPTIONS.get(c["name"], "") + (f" Наследует `{c['base']}`." if c["base"] and c["base"] in names and c["base"] not in DESCRIPTIONS.get(c["name"], "") else ""))
        out.append("")
        if c["fields"]:
            out.append("**Поля:** " + "; ".join(f"`{n}` — `{t}`" for n, t in c["fields"]))
            out.append("")
        out.append("**Методы:** " + (", ".join(f"`{b}`" for b in c["behaviors"]) if c["behaviors"] else "—"))
        out.append("")
        out.append("**Ассоциации:** " + (", ".join(c["related"]) if c["related"] else "—"))
        out.append("")

    with open(os.path.join(ROOT, "README.md"), "w", encoding="utf-8") as f:
        f.write("\n".join(out))
    print(f"classes={len(domain)} exceptions={len(exceptions)} fields={total_f} behaviors={total_b} associations={assoc_pairs}")
    if len(domain) < 50 or total_f < 150 or total_b < 100 or assoc_pairs < 30 or len(exceptions) < 12:
        print("ВНИМАНИЕ: требования задания не выполнены!")
        sys.exit(1)


if __name__ == "__main__":
    main()
