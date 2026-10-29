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
    "Address": "Почтовый адрес.",
    "AgencyManager": "Руководитель агентства: одобряет возвраты в рамках лимита, разблокирует аккаунты.",
    "Aircraft": "Воздушное судно: число мест по классам, норма багажа.",
    "Airline": "Авиакомпания с рейтингом надёжности.",
    "Airport": "Аэропорт с кодом IATA (3 заглавные буквы).",
    "Attraction": "Достопримечательность с часами работы и ценой билета.",
    "Booking": "Базовое бронирование: статусы, отмена со штрафом, полиморфные условия отмены.",
    "BookingManager": "Реестр бронирований: поиск, отмена, статистика, выручка.",
    "CabinClass": "Класс обслуживания (перечисление, вынесено в отдельный заголовок).",
    "CarRental": "Автомобиль в аренду.",
    "CarRentalBooking": "Бронирование аренды авто (наследник Booking).",
    "City": "Город со страной, координатами и часовым поясом.",
    "ContactInfo": "Телефон и e-mail с проверкой формата.",
    "Country": "Страна: валюта и правила безвизового въезда.",
    "CurrencyConverter": "Конвертер валют через базовую валюту.",
    "Date": "Календарная дата с арифметикой и днём недели.",
    "DateRange": "Период между двумя датами.",
    "Excursion": "Экскурсия: гид, достопримечательность, вместимость.",
    "ExcursionBooking": "Бронирование экскурсии (наследник Booking).",
    "Flight": "Рейс: самолёт, места, цена по классам, время прилёта.",
    "FlightBooking": "Бронирование авиабилета (наследник Booking).",
    "GeoPoint": "Географическая точка, расстояние по формуле гаверсинусов.",
    "Guide": "Гид-экскурсовод (наследник Staff).",
    "Hotel": "Отель: номера, удобства, бронирование и отмена номера.",
    "HotelBooking": "Бронирование проживания (наследник Booking).",
    "InsuranceClaim": "Заявка на страховой случай.",
    "InsurancePolicy": "Страховой полис: тип, покрытие, подача и одобрение заявок.",
    "Itinerary": "Маршрут путешествия из нескольких бронирований, поиск пересечений.",
    "LoyaltyAccount": "Бонусный счёт: баллы, уровни, скидки.",
    "Money": "Денежная сумма в копейках с валютой (без ошибок округления double).",
    "Notification": "Уведомление пользователю.",
    "NotificationService": "Отправка и хранение уведомлений, очистка устаревших.",
    "Passport": "Паспорт: формат номера, срок действия, проверка на поездку.",
    "Payment": "Платёж с жизненным циклом статусов и частичными возвратами.",
    "PaymentProcessor": "Приём платежей, лимиты, комиссия, возвраты.",
    "Person": "Базовый класс человека.",
    "PriceCalculator": "Сезонные цены, наценка за выходные, групповые скидки.",
    "PromoCode": "Промокод со сроком действия, минимальной суммой и лимитом.",
    "RefundCalculator": "Расчёт возврата с учётом штрафа, комиссии и окна возврата.",
    "Review": "Отзыв с оценкой 1–5.",
    "ReviewBoard": "Доска отзывов: модерация, дубликаты, средние оценки.",
    "Room": "Номер отеля: даты занятости, цена за ночь.",
    "Seat": "Место в самолёте.",
    "Staff": "Сотрудник агентства (наследник Person).",
    "SupportOperator": "Оператор поддержки с ограничением нагрузки (наследник Staff).",
    "SupportTicket": "Обращение в поддержку с жизненным циклом статусов.",
    "TourPackage": "Готовый тур: перелёты, отель, экскурсии, проверка согласованности.",
    "TourBooking": "Бронирование тура (наследник Booking).",
    "TravelAdvisory": "Предупреждение о безопасности поездки в страну.",
    "TravelAgency": "Фасад: аккаунты, бронирование, оплата, отмена, страховки, поддержка.",
    "TravelAgent": "Турагент с комиссией (наследник Staff).",
    "TravelAnalytics": "Аналитика по бронированиям: выручка, отмены, популярные направления.",
    "TravelCatalog": "Каталог рейсов, отелей, туров, достопримечательностей и предупреждений.",
    "TravelPreferences": "Предпочтения путешественника: бюджет, класс, пересадки.",
    "Traveler": "Путешественник: паспорт, гражданство, визы (наследник Person).",
    "UserAccount": "Учётная запись: проверка пароля, блокировка после 3 ошибок.",
    "Visa": "Виза: страна, срок, число въездов.",
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
    for m in re.finditer(r"\bclass\s+(\w+)\s*(?::\s*public\s+([\w:]+))?\s*\{", text):
        if text[:m.start()].rstrip().endswith("enum"):
            continue
        end = match_brace(text, m.end() - 1)
        body = text[m.end():end]
        cls = {"name": m.group(1), "base": m.group(2), "fields": [], "methods": []}
        for stmt in split_members(body):
            stmt = re.sub(r"^(public|private|protected)\s*:\s*", "", stmt).strip()
            if not stmt or stmt.startswith(("using", "friend", "static_assert")):
                continue
            if "(" in stmt:
                head = stmt[:stmt.index("(")]
                name_m = re.search(r"(~?\w+)\s*$", head)
                if not name_m:
                    continue
                params = stmt[stmt.index("(") + 1:match_paren(stmt, stmt.index("("))]
                cls["methods"].append((name_m.group(1), " ".join(params.split())))
            else:
                decl = stmt.split("=")[0].strip()
                name_m = re.search(r"(\w+_)\s*$", decl)
                if name_m:
                    cls["fields"].append((name_m.group(1), decl[:name_m.start()].strip()))
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
        c["behaviors"] = sorted({n for n, _ in c["methods"] if is_behavior(c, n)})
        related = set()
        for t in [t for _, t in c["fields"]] + [p for _, p in c["methods"]]:
            related |= set(re.findall(r"\b\w+\b", t)) & names
        if c["base"] in names:
            related.add(c["base"])
        related.discard(c["name"])
        c["related"] = sorted(related)
        total_f += len(c["fields"])
        total_b += len(c["behaviors"])
        assoc_pairs += len(related)
        rows.append(f"| {c['name']} | {len(c['fields'])} | {len(c['behaviors'])} | "
                    f"{', '.join(c['related']) if related else '—'} |")

    exc_fields = sum(len(c["fields"]) for c in exceptions)
    out = []
    out.append("# Лабораторная работа №3. Система бронирования путешествий\n")
    out.append("Предметная область: **система бронирования путешествий** (турагентство). Модель описывает путешественников "
               "с паспортами и визами, учётные записи с проверкой пароля и блокировкой, географию (страны, города, аэропорты), "
               "авиарейсы с местами и классами обслуживания, отели с номерами, туры и экскурсии, аренду автомобилей, "
               "бронирования разных типов (с полиморфными условиями отмены), платежи, промокоды, бонусную программу, "
               "страхование, поддержку клиентов, персонал агентства и аналитику.\n")
    out.append("Язык: **C++17**. Все классы лежат в заголовочных файлах `include/*.h` (по одному классу на файл), "
               "консольное приложение и сценарии работы с классами — в `src/main.cpp`.\n")
    out.append("## Структура репозитория\n")
    out.append("```\ninclude/      классы предметной области (.h) и exceptions.h\nsrc/main.cpp  консольное меню и демо-сценарии\n"
               "tests/        модульные тесты (tests/tests.cpp)\ntools/        gen_readme.py — генератор этого README\n"
               "CMakeLists.txt\n```\n")
    out.append("## Сборка и запуск\n")
    out.append("```bash\ncmake -S . -B build\ncmake --build build\n./build/travel_booking          # интерактивное меню\n"
               "./build/travel_booking --demo   # автоматический демо-сценарий\n./build/run_tests             # тесты\n```\n")
    out.append("Без CMake: `g++ -std=c++17 -Iinclude src/main.cpp -o travel_booking`. "
               "В Windows консоль переключается в UTF-8 автоматически.\n")
    out.append("## Консольное меню\n")
    out.append("Поиск рейсов, отелей и туров; вход в аккаунт (демо-пользователи `ivan`/`Ivan2026pass`, `maria`/`Maria2026pass`, "
               "`oleg`/`Oleg2026pass`); бронирование перелёта, отеля, тура, автомобиля и экскурсии; маршрут с поиском "
               "пересечений; оплата с промокодом (`WELCOME10`) и бонусами; отмена со штрафом и возвратом; страхование; "
               "поддержка; отзывы; уведомления; конвертер валют; аналитика; демо-сценарий и демонстрация всех исключений.\n")
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
        base = c["base"]
        out.append(f"- {c['name']}" + (f" (наследник {base})" if base and base != "std::runtime_error" else ""))
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
               f"Поля исключений ({exc_fields} шт.) в общее число не входят. Поведения подсчитаны построчно по каждому классу, "
               "перегруженные методы с одним именем считаются один раз.\n")
    out.append("## Подробное описание классов\n")
    for c in domain:
        out.append(f"### {c['name']}\n")
        desc = DESCRIPTIONS.get(c["name"], "")
        out.append(desc)
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
    missing = [c["name"] for c in domain if c["name"] not in DESCRIPTIONS]
    if missing:
        print("Нет описания для:", ", ".join(missing))
    if len(domain) < 50 or total_f < 150 or total_b < 100 or assoc_pairs < 30 or len(exceptions) < 12:
        print("ВНИМАНИЕ: требования задания не выполнены!")
        sys.exit(1)


if __name__ == "__main__":
    main()
