// Консольное приложение «Книжный склад».
// Все классы предметной области находятся в include/*.h, здесь — только консоль и сценарии работы.
#include <ctime>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#include "Accountant.h"
#include "BarcodeScanner.h"
#include "Discount.h"
#include "Forklift.h"
#include "InboundShipment.h"
#include "InventoryAudit.h"
#include "Loader.h"
#include "OutboundShipment.h"
#include "Pallet.h"
#include "Picker.h"
#include "ProcurementManager.h"
#include "ReportGenerator.h"
#include "Reservation.h"
#include "ReturnRequest.h"
#include "Schedule.h"
#include "Storekeeper.h"
#include "SupplierInvoice.h"
#include "TaxCalculator.h"
#include "WarehouseManager.h"
#include "WarehouseSystem.h"

namespace {

bool g_eof = false;

std::string readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string s;
    if (!std::getline(std::cin, s)) {
        g_eof = true;
        return "";
    }
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ')) s.pop_back();
    return s;
}

int readInt(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::string s = readLine(prompt);
        if (g_eof) return lo;
        try {
            size_t pos = 0;
            int value = std::stoi(s, &pos);
            if (pos == s.size() && value >= lo && value <= hi) return value;
        } catch (const std::exception&) {
        }
        std::cout << "Введите число от " << lo << " до " << hi << ".\n";
    }
}

Date todayDate() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

Employee makeEmployee(const std::string& id, const std::string& first, const std::string& last, int birthYear,
                      const std::string& position, double salary, const Date& hired) {
    ContactInfo contact("+7 (900) 000-00-" + id.substr(1), "emp" + id + "@books.example");
    return Employee(id, first, last, contact, Date(birthYear, 3, 15), position, Money::fromUnits(salary), hired);
}

struct Staff {
    WarehouseManager manager;
    ProcurementManager buyer;
    Storekeeper keeperFiction;
    Storekeeper keeperTech;
    Picker picker;
    Loader loader;
    Accountant accountant;
};

Staff makeStaff(const Date& today) {
    Date hired = today.addDays(-900);
    return Staff{
        WarehouseManager(makeEmployee("E01", "Алексей", "Смирнов", 1982, "Заведующий складом", 120000, hired), "W1",
                         Money::fromUnits(50000)),
        ProcurementManager(makeEmployee("E02", "Ольга", "Кузнецова", 1988, "Менеджер по закупкам", 95000, hired),
                           Money::fromUnits(500000)),
        Storekeeper(makeEmployee("E03", "Игорь", "Волков", 1991, "Кладовщик", 65000, hired), "Z-A",
                    BarcodeScanner("SC-1")),
        Storekeeper(makeEmployee("E04", "Елена", "Морозова", 1993, "Кладовщик", 65000, hired), "Z-B",
                    BarcodeScanner("SC-2")),
        Picker(makeEmployee("E05", "Дмитрий", "Орлов", 1995, "Комплектовщик", 55000, hired), 60),
        Loader(makeEmployee("E06", "Сергей", "Попов", 1990, "Грузчик", 50000, hired), 25),
        Accountant(makeEmployee("E07", "Наталья", "Фёдорова", 1985, "Бухгалтер", 85000, hired), Money::fromUnits(1000000))};
}

Warehouse makeWarehouse() {
    Warehouse warehouse("W1", "Центральный книжный склад", Address("Промышленная ул., 12", "Москва", "115000", "Россия"));
    Zone fiction("Z-A", "Художественная и детская литература");
    Zone tech("Z-B", "Техническая литература");
    for (int i = 1; i <= 3; ++i) fiction.addShelf(Shelf("A-" + std::to_string(i), 200000));
    for (int i = 1; i <= 2; ++i) tech.addShelf(Shelf("B-" + std::to_string(i), 150000));
    warehouse.addZone(fiction);
    warehouse.addZone(tech);
    warehouse.addDock(LoadingDock("D1"));
    warehouse.addDock(LoadingDock("D2"));
    return warehouse;
}

class ConsoleApp {
    Date today_;
    WarehouseSystem system_;
    Staff staff_;
    BarcodeScanner scanner_;
    Forklift forklift_;
    Schedule schedule_;
    TaxCalculator tax_;
    std::vector<Discount> discounts_;
    std::vector<ShippingCarrier> carriers_;
    std::vector<OutboundShipment> shipments_;
    std::vector<ReturnRequest> returns_;
    std::vector<Reservation> reservations_;
    std::vector<PayrollRecord> payroll_;
    int nextShipmentId_ = 1;
    int nextPackageId_ = 1;
    int nextReturnId_ = 1;
    int nextReservationId_ = 1;
    int nextAuditId_ = 1;
    int nextInvoiceId_ = 1;
    int nextPalletId_ = 1;

public:
    ConsoleApp()
        : today_(todayDate()),
          system_(makeWarehouse(), PricingPolicy(35.0, 10.0, 0.0)),
          staff_(makeStaff(today_)),
          scanner_("SC-0"),
          forklift_("FL-1", 1000),
          schedule_(today_) {
        seedData();
    }

    // ------------------------------------------------------------------ данные

    void seedData() {
        Publisher ast("P1", "АСТ", Address("Звёздный б-р, 21", "Москва", "129085", "Россия"), "97851", 1990);
        Publisher eksmo("P2", "Эксмо", Address("Зорге, 1", "Москва", "125252", "Россия"), "97855", 1991);
        Publisher azbuka("P3", "Азбука", Address("Невский пр., 2", "Санкт-Петербург", "191000", "Россия"), "97856", 1995);
        Publisher piter("P4", "Питер", Address("Лиговский пр., 10", "Санкт-Петербург", "191040", "Россия"), "97854", 1991);
        Publisher prosv("P5", "Просвещение", Address("Академика Королёва, 13", "Москва", "127000", "Россия"), "97850", 1930);

        Genre classic("G1", "Классика", 12);
        Genre prog("G2", "Программирование", 0);
        Genre kids("G3", "Детская литература", 0);
        system_.getCatalog().registerGenre(classic);
        system_.getCatalog().registerGenre(prog);
        system_.getCatalog().registerGenre(kids);

        Author dost("A1", "Фёдор Достоевский", "Россия", 1821);
        Author bulg("A2", "Михаил Булгаков", "Россия", 1891);
        Author tolst("A3", "Лев Толстой", "Россия", 1828);
        Author stroustrup("A4", "Бьёрн Страуструп", "Дания", 1950);
        Author nosov("A5", "Николай Носов", "Россия", 1908);
        Author martin("A6", "Роберт Мартин", "США", 1952);

        std::vector<Book> books;
        books.emplace_back(ISBN("9785170123452"), "Преступление и наказание", dost, ast, classic, Money::fromUnits(450), 520, 608, 2019);
        books.emplace_back(ISBN("9785517000019"), "Мастер и Маргарита", bulg, eksmo, classic, Money::fromUnits(520), 640, 480, 2021);
        books.emplace_back(ISBN("9785699012343"), "Война и мир", tolst, azbuka, classic, Money::fromUnits(990), 1500, 1360, 2018);
        books.emplace_back(ISBN("9785446111114"), "Язык программирования C++", stroustrup, piter, prog, Money::fromUnits(4200), 1800, 1136, 2022);
        books.emplace_back(ISBN("9785012345677"), "Приключения Незнайки", nosov, prosv, kids, Money::fromUnits(380), 450, 320, 2020);
        books.emplace_back(ISBN("9785457000025"), "Чистый код", martin, piter, prog, Money::fromUnits(2900), 800, 464, 2020);

        const int initial[] = {40, 30, 15, 10, 50, 12};
        for (size_t i = 0; i < books.size(); ++i) {
            system_.addBook(books[i], 5, "-", today_);
            placeStock(books[i], initial[i], staff_.keeperFiction);
        }

        Supplier s1("S1", "ТД Книжный мир", Address("Складская, 5", "Москва", "109000", "Россия"),
                    ContactInfo("+7 (495) 111-22-33", "sales@bookworld.example"));
        Supplier s2("S2", "Оптовик Книга", Address("Базовая, 7", "Тверь", "170000", "Россия"),
                    ContactInfo("+7 (482) 222-33-44", "opt@kniga.example"));
        for (const auto& b : books) {
            s1.addPrice(b.getIsbn(), b.getPrice().percentOf(62.0));
            s2.addPrice(b.getIsbn(), b.getPrice().percentOf(trimmedFactor(b)));
        }
        s1.rate(4.6);
        s2.rate(3.7);
        system_.addSupplier(s1);
        system_.addSupplier(s2);

        system_.registerCustomer(Customer("C001", "Иван", "Петров", ContactInfo("+7 (911) 123-45-67", "ivan@example.com"),
                                          Date(1990, 5, 12), Address("Ленина, 10", "Москва", "101000", "Россия"),
                                          LoyaltyCard("LC-001", today_)));
        system_.registerCustomer(Customer("C002", "Мария", "Сидорова", ContactInfo("+7 (922) 765-43-21", "maria@example.com"),
                                          Date(1985, 11, 3), Address("Мира, 5", "Казань", "420000", "Россия"),
                                          LoyaltyCard("LC-002", today_)));
        system_.registerCustomer(Customer("C003", "Саша", "Иванов", ContactInfo("+7 (933) 000-11-22", "sasha@example.com"),
                                          Date(2016, 7, 1), Address("Садовая, 1", "Тула", "300000", "Россия"),
                                          LoyaltyCard("LC-003", today_)));

        discounts_.emplace_back("BOOK10", 10.0, Money::fromUnits(1000), today_.addDays(30), 100);
        discounts_.emplace_back("OLD2020", 15.0, Money(), Date(2020, 12, 31), 10);

        carriers_.emplace_back("Быстрая почта", Money::fromUnits(200), Money::fromUnits(50), 10000, 2);
        carriers_.emplace_back("Грузовая линия", Money::fromUnits(500), Money::fromUnits(20), 100000, 5);

        staff_.loader.certifyForklift();
        scanner_.recharge();
    }

    static double trimmedFactor(const Book& b) { return b.getPrice().getCents() > 100000 ? 58.0 : 66.0; }

    Zone& zoneFor(const Book& book) {
        return system_.getWarehouse().findZone(book.getGenre().getName() == "Программирование" ? "Z-B" : "Z-A");
    }

    Storekeeper& keeperFor(const Book& book) {
        return book.getGenre().getName() == "Программирование" ? staff_.keeperTech : staff_.keeperFiction;
    }

    // Физически размещает книги на стеллаже и учитывает приход в инвентаре.
    void placeStock(const Book& book, int quantity, Storekeeper& by) {
        std::string shelf = keeperFor(book).placeBook(zoneFor(book), book, quantity);
        Inventory& inventory = system_.getWarehouse().getInventory();
        inventory.receive(book.getIsbn(), quantity, by, today_);
        inventory.findItem(book.getIsbn()).moveToShelf(shelf);
    }

    // ------------------------------------------------------------ сценарии

    // Закупка у самого дешёвого поставщика с приёмкой и оплатой счёта.
    void restock(const Book& book, int quantity) {
        const Supplier& supplier = staff_.buyer.chooseCheapestSupplier(system_.getSuppliers(), book.getIsbn());
        std::string supplierName = supplier.getName();
        std::cout << "Выбран поставщик: " << supplierName << " (цена " << supplier.priceFor(book.getIsbn()).toString() << ")\n";

        int poId = system_.createPurchase(supplier.getId(), book.getIsbn(), quantity, today_);
        PurchaseOrder& po = system_.findPurchase(poId);
        staff_.buyer.approveOrder(po);
        po.markSent();
        std::cout << "Закупка №" << poId << " на " << po.totalCost().toString()
                  << " утверждена. Остаток бюджета: " << staff_.buyer.getBudget().toString() << "\n";

        InboundShipment shipment(nextShipmentId_++, po, today_);
        LoadingDock& dock = system_.getWarehouse().acquireFreeDock(supplierName, today_);
        shipment.assignDock(dock);
        shipment.registerReceived(book.getIsbn(), quantity);
        Storekeeper& keeper = keeperFor(book);
        shipment.inspect(keeper);
        std::string shelf = keeper.placeBook(zoneFor(book), book, quantity);
        system_.receivePurchase(poId, keeper, today_);
        system_.getWarehouse().getInventory().findItem(book.getIsbn()).moveToShelf(shelf);
        dock.release();
        std::cout << "Поставка принята у дока " << shipment.getDockId() << ", размещена на стеллаже " << shelf
                  << (shipment.isComplete() ? " (комплектна)" : " (не комплектна)") << "\n";

        SupplierInvoice invoice("INV-" + std::to_string(nextInvoiceId_++), poId, po.totalCost(), today_, today_.addDays(14));
        Payment payment(poId, po.totalCost(), PaymentMethod::BankTransfer, today_);
        payment.process();
        staff_.accountant.payInvoice(invoice, payment);
        std::cout << "Счёт " << invoice.getId() << " оплачен. Баланс кассы: " << staff_.accountant.getLedgerBalance().toString() << "\n";
    }

    Discount& findDiscount(const std::string& code) {
        for (auto& d : discounts_) {
            if (d.getCode() == code) return d;
        }
        throw InvalidDataException("промокод не найден: " + code);
    }

    int placeOrder(const std::string& customerId, const std::vector<std::pair<ISBN, int>>& items,
                   const std::string& discountCode) {
        int id = system_.createOrder(customerId, today_);
        try {
            for (const auto& item : items) system_.addToOrder(id, item.first, item.second);
            if (!discountCode.empty()) system_.findOrder(id).applyDiscount(findDiscount(discountCode), today_);
            system_.confirmOrder(id, today_);
        } catch (...) {
            try {
                system_.cancelOrder(id, today_);
            } catch (...) {
            }
            throw;
        }
        return id;
    }

    ShippingCarrier cheapestCarrier(const Package& package) const {
        const ShippingCarrier* best = nullptr;
        for (const auto& c : carriers_) {
            if (c.canCarry(package) && (best == nullptr || best->calculateCost(package).isGreaterThan(c.calculateCost(package)))) {
                best = &c;
            }
        }
        if (best == nullptr) throw ShelfOverloadException("ни один перевозчик не берёт посылку");
        return *best;
    }

    void payAndShip(int orderId, PaymentMethod method) {
        CustomerOrder& order = system_.findOrder(orderId);
        if (order.getStatus() != OrderStatus::Confirmed) {
            throw InvalidOrderStateException("оплатить и отгрузить", toString(order.getStatus()));
        }
        Package package(nextPackageId_++, orderId);
        staff_.picker.pickOrder(order, package, system_.getCatalog());
        package.seal();
        ShippingCarrier carrier = cheapestCarrier(package);

        if (!order.isPaid()) {
            Payment payment = system_.payOrder(orderId, method, today_);
            staff_.accountant.recordIncome(payment.getAmount());
            std::cout << "Оплата (" << toString(payment.getMethod()) << "): " << payment.getAmount().toString() << "\n";
        }
        system_.fulfillOrder(orderId, staff_.picker, today_);
        for (const auto& l : order.getLines()) {
            const Book& book = system_.getCatalog().findByIsbn(l.getIsbn());
            keeperFor(book).takeBook(zoneFor(book), l.getIsbn(), l.getQuantity());
        }
        OutboundShipment shipment(nextShipmentId_++, package, carrier, order.getShippingAddress());
        shipment.dispatch(today_);
        system_.shipOrder(orderId, today_);
        std::cout << "Отгружено перевозчиком «" << carrier.getName() << "», трек " << shipment.getTrackingNumber()
                  << ", вес " << package.weightKg() << " кг, доставка " << shipment.cost().toString()
                  << ", ожидается " << shipment.eta().toString() << "\n";
        shipments_.push_back(shipment);
    }

    void confirmDelivery(int orderId) {
        for (auto& s : shipments_) {
            if (s.getPackage().getOrderId() == orderId && s.getStatus() == ShipmentStatus::InTransit) {
                s.markDelivered();
                system_.findOrder(orderId).deliver();
                std::cout << "Заказ №" << orderId << " доставлен.\n";
                return;
            }
        }
        throw InvalidOrderStateException("подтвердить доставку", "заказ не в пути");
    }

    void returnItem(int orderId, const ISBN& isbn, int quantity, const std::string& reason) {
        CustomerOrder& order = system_.findOrder(orderId);
        if (order.getStatus() != OrderStatus::Shipped && order.getStatus() != OrderStatus::Delivered) {
            throw InvalidOrderStateException("оформить возврат", toString(order.getStatus()));
        }
        Money unitPrice;
        bool found = false;
        for (const auto& l : order.getLines()) {
            if (l.getIsbn().equals(isbn) && l.getQuantity() >= quantity) {
                unitPrice = l.getUnitPrice();
                found = true;
            }
        }
        if (!found) throw BookNotFoundException(isbn.getCode() + " в количестве " + std::to_string(quantity) + " нет в заказе");
        ReturnRequest request(nextReturnId_++, orderId, isbn, quantity, reason, today_);
        if (!request.isWithinReturnWindow(order.getCreatedAt(), 14)) {
            request.reject();
            std::cout << "Возврат отклонён: истёк срок 14 дней.\n";
        } else {
            request.approve();
            const Book& book = system_.getCatalog().findByIsbn(isbn);
            placeStock(book, quantity, keeperFor(book));
            request.complete();
            std::cout << "Возврат принят, сумма возмещения " << request.refundAmount(unitPrice).toString() << "\n";
            staff_.accountant.recordExpense(request.refundAmount(unitPrice));
        }
        returns_.push_back(request);
    }

    // ----------------------------------------------------------- интерфейс

    std::optional<Book> chooseBook(const std::string& prompt) {
        std::vector<Book> books = system_.getCatalog().allBooks();
        for (size_t i = 0; i < books.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << books[i].getTitle() << " — " << books[i].getAuthor().getName() << "\n";
        }
        int n = readInt(prompt + " (0 — отмена): ", 0, static_cast<int>(books.size()));
        if (n == 0) return std::nullopt;
        return books[static_cast<size_t>(n) - 1];
    }

    std::string chooseCustomer() {
        const auto& customers = system_.getCustomers();
        for (size_t i = 0; i < customers.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << customers[i].fullName() << " (" << customers[i].getId() << ", "
                      << toString(customers[i].getLoyaltyCard().getTier()) << ", возраст "
                      << customers[i].ageOn(today_) << ")\n";
        }
        int n = readInt("Клиент (0 — отмена): ", 0, static_cast<int>(customers.size()));
        return n == 0 ? "" : customers[static_cast<size_t>(n) - 1].getId();
    }

    void showCatalog() {
        const Inventory& inventory = system_.getWarehouse().getInventory();
        for (const auto& b : system_.getCatalog().allBooks()) {
            std::cout << b.getIsbn().formatted() << " | " << b.getTitle() << " | " << b.getAuthor().getName() << " | "
                      << b.getGenre().getName() << " (" << b.getGenre().getMinAge() << "+) | " << b.getPrice().toString()
                      << " | доступно: " << inventory.getItem(b.getIsbn()).available() << "\n";
        }
    }

    void searchBooks() {
        std::string query = readLine("Строка поиска (название или автор): ");
        auto found = system_.getCatalog().search(query);
        if (found.empty()) std::cout << "Ничего не найдено.\n";
        for (const auto& b : found) {
            std::cout << b.getTitle() << " — " << b.getAuthor().getName() << ", " << b.getPublisher().getName() << ", "
                      << b.getPublicationYear() << " г., " << b.getPages() << " стр., " << b.getPrice().toString()
                      << (b.isOldEdition(today_.getYear(), 5) ? " [старое издание]" : "") << "\n";
        }
    }

    void stockMenu() {
        Inventory& inventory = system_.getWarehouse().getInventory();
        std::cout << "1. Остатки и места хранения\n2. Найти место книги\n3. Сканировать штрихкод\n"
                     "4. Списать книги\n5. Погрузить паллету\n0. Назад\n";
        int c = readInt("> ", 0, 5);
        if (c == 1) {
            for (const auto& b : system_.getCatalog().allBooks()) {
                const StockItem& item = inventory.getItem(b.getIsbn());
                std::cout << b.getTitle() << ": всего " << item.getQuantity() << ", резерв " << item.getReserved()
                          << ", доступно " << item.available() << ", стеллаж " << item.getShelfId()
                          << (item.needsReorder() ? " [нужна закупка]" : "") << "\n";
            }
        } else if (c == 2) {
            auto book = chooseBook("Книга");
            if (book) std::cout << "Место хранения: " << system_.getWarehouse().locateBook(book->getIsbn()) << "\n";
        } else if (c == 3) {
            std::string code = readLine("Код (ISBN-13): ");
            ISBN isbn = scanner_.scan(code);
            const Book& book = system_.getCatalog().findByIsbn(isbn);
            std::cout << "Отсканировано: " << book.getTitle() << " (" << book.getPublisher().getName() << "), российская группа: "
                      << (isbn.isRussianGroup() ? "да" : "нет") << ", издатель владеет кодом: "
                      << (book.getPublisher().ownsIsbn(isbn) ? "да" : "нет") << "\n";
        } else if (c == 4) {
            auto book = chooseBook("Книга для списания");
            if (!book) return;
            int qty = readInt("Количество: ", 1, 1000);
            std::string reason = readLine("Причина: ");
            WriteOffRecord draft(0, book->getIsbn(), qty, reason, today_, staff_.manager.getId());
            Money unitCost = book->getPrice().percentOf(60.0);
            if (!staff_.manager.approveWriteOff(draft, unitCost)) {
                throw AccessDeniedException(staff_.manager.fullName(), "списание на сумму " + draft.lossAmount(unitCost).toString());
            }
            inventory.writeOff(book->getIsbn(), qty, reason, staff_.manager, today_);
            keeperFor(*book).takeBook(zoneFor(*book), book->getIsbn(), qty);
            std::cout << "Списано. Потери: " << draft.lossAmount(unitCost).toString() << (draft.isDamage() ? " (порча)" : "") << "\n";
        } else if (c == 5) {
            auto book = chooseBook("Книга для паллеты");
            if (!book) return;
            int qty = readInt("Количество: ", 1, 5000);
            Pallet pallet("PL-" + std::to_string(nextPalletId_++), 800000);
            staff_.loader.loadPallet(pallet, *book, qty);
            forklift_.assignOperator(staff_.loader);
            forklift_.liftPallet(pallet);
            std::cout << "Паллета " << pallet.getId() << ": " << pallet.bookCount() << " шт., " << pallet.getWeightGrams() / 1000.0
                      << " кг, заряд погрузчика " << forklift_.getBatteryPercent() << "%\n";
            forklift_.releaseOperator();
        }
    }

    void procurementMenu() {
        auto book = chooseBook("Книга для закупки");
        if (!book) return;
        int qty = readInt("Количество (1-500): ", 1, 500);
        restock(*book, qty);
    }

    void newOrderMenu() {
        std::string customerId = chooseCustomer();
        if (customerId.empty()) return;
        std::vector<std::pair<ISBN, int>> items;
        while (true) {
            auto book = chooseBook("Добавить книгу в заказ");
            if (!book) break;
            items.emplace_back(book->getIsbn(), readInt("Количество: ", 1, 1000));
        }
        std::string code = readLine("Промокод (Enter — без скидки): ");
        int id = placeOrder(customerId, items, code);
        printOrder(system_.findOrder(id));
    }

    void printOrder(const CustomerOrder& order) {
        std::cout << "Заказ №" << order.getId() << " [" << toString(order.getStatus()) << "], клиент "
                  << order.getCustomer().fullName() << ", адрес: " << order.getShippingAddress().fullAddress() << "\n";
        for (const auto& l : order.getLines()) {
            std::cout << "  " << system_.getCatalog().findByIsbn(l.getIsbn()).getTitle() << " x" << l.getQuantity()
                      << " = " << l.total().toString() << "\n";
        }
        std::cout << "  Сумма: " << order.subtotal().toString() << ", скидка " << order.getDiscountPercent() << "%, к оплате "
                  << order.total().toString() << " (в т.ч. НДС " << tax_.vatIncluded(order.total(), true).toString() << ")\n";
    }

    int chooseOrder(const std::string& prompt) {
        const auto& orders = system_.getOrders();
        for (const auto& o : orders) {
            std::cout << "  №" << o.getId() << " " << o.getCustomer().fullName() << " [" << toString(o.getStatus()) << "] "
                      << o.total().toString() << "\n";
        }
        if (orders.empty()) {
            std::cout << "Заказов пока нет.\n";
            return 0;
        }
        return readInt(prompt + " (0 — отмена): ", 0, 100000);
    }

    void shipMenu() {
        int id = chooseOrder("Номер заказа для оплаты и отгрузки");
        if (id == 0) return;
        std::cout << "Способ оплаты: 1 наличные, 2 карта, 3 перевод, 4 онлайн\n";
        int m = readInt("> ", 1, 4);
        payAndShip(id, static_cast<PaymentMethod>(m - 1));
    }

    void deliveryMenu() {
        int id = chooseOrder("Номер заказа, который доставлен");
        if (id != 0) confirmDelivery(id);
    }

    void returnsMenu() {
        int id = chooseOrder("Номер заказа для возврата");
        if (id == 0) return;
        CustomerOrder& order = system_.findOrder(id);
        const auto& lines = order.getLines();
        for (size_t i = 0; i < lines.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << system_.getCatalog().findByIsbn(lines[i].getIsbn()).getTitle() << " x"
                      << lines[i].getQuantity() << "\n";
        }
        int n = readInt("Позиция (0 — отмена): ", 0, static_cast<int>(lines.size()));
        if (n == 0) return;
        ISBN isbn = lines[static_cast<size_t>(n) - 1].getIsbn();
        int qty = readInt("Количество: ", 1, 1000);
        returnItem(id, isbn, qty, readLine("Причина возврата: "));
    }

    void reservationsMenu() {
        Inventory& inventory = system_.getWarehouse().getInventory();
        std::cout << "1. Создать резерв\n2. Список резервов\n3. Продлить резерв\n4. Отменить резерв\n0. Назад\n";
        int c = readInt("> ", 0, 4);
        if (c == 1) {
            std::string customerId = chooseCustomer();
            if (customerId.empty()) return;
            auto book = chooseBook("Книга");
            if (!book) return;
            int qty = readInt("Количество: ", 1, 100);
            inventory.reserve(book->getIsbn(), qty);
            reservations_.emplace_back(nextReservationId_++, customerId, book->getIsbn(), qty, today_.addDays(3));
            std::cout << "Резерв №" << reservations_.back().getId() << " до " << reservations_.back().getExpiresAt().toString() << "\n";
        } else if (c == 2) {
            for (const auto& r : reservations_) {
                std::cout << "№" << r.getId() << " клиент " << r.getCustomerId() << " " << r.getIsbn().formatted() << " x"
                          << r.getQuantity() << " до " << r.getExpiresAt().toString()
                          << (r.getActive() ? (r.isExpired(today_) ? " [просрочен]" : " [активен]") : " [отменён]") << "\n";
            }
            if (reservations_.empty()) std::cout << "Резервов нет.\n";
        } else if (c == 3 || c == 4) {
            int id = readInt("Номер резерва: ", 1, 100000);
            for (auto& r : reservations_) {
                if (r.getId() != id) continue;
                if (c == 3) {
                    r.extend(readInt("На сколько дней: ", 1, 30));
                } else if (r.getActive()) {
                    r.cancel();
                    inventory.releaseReservation(r.getIsbn(), r.getQuantity());
                }
                std::cout << "Готово.\n";
                return;
            }
            std::cout << "Резерв не найден.\n";
        }
    }

    void staffMenu() {
        std::vector<const Employee*> staff = {&staff_.manager, &staff_.buyer, &staff_.keeperFiction, &staff_.keeperTech,
                                              &staff_.picker, &staff_.loader, &staff_.accountant};
        std::cout << "1. Список сотрудников\n2. Назначить смену\n3. Расчёт зарплаты\n0. Назад\n";
        int c = readInt("> ", 0, 3);
        if (c == 1) {
            for (const auto* e : staff) {
                std::cout << e->getId() << " " << e->fullName() << " — " << e->role() << ", оклад " << e->getSalary().toString()
                          << ", стаж " << e->yearsOfService(today_) << " г."
                          << (e->canApproveOrders() ? ", утверждает закупки/списания" : "") << ", часов в графике: "
                          << schedule_.hoursFor(e->getId()) << "\n";
            }
        } else if (c == 2) {
            for (size_t i = 0; i < staff.size(); ++i) std::cout << "  " << i + 1 << ". " << staff[i]->fullName() << "\n";
            int n = readInt("Сотрудник: ", 1, static_cast<int>(staff.size()));
            int from = readInt("Начало смены (час 0-23): ", 0, 23);
            int to = readInt("Конец смены (час 1-24): ", 1, 24);
            Shift shift(staff[static_cast<size_t>(n) - 1]->getId(), today_, from, to);
            staff_.manager.assignShift(schedule_, shift);
            std::cout << "Смена назначена (" << shift.durationHours() << " ч" << (shift.isNightShift() ? ", ночная" : "") << ").\n";
        } else if (c == 3) {
            payroll_.clear();
            std::string period = today_.toString().substr(0, 7);
            for (const auto* e : staff) {
                Money bonus = schedule_.hoursFor(e->getId()) > 8 ? Money::fromUnits(2000) : Money();
                payroll_.push_back(staff_.accountant.buildPayroll(*e, period, bonus, tax_));
                payroll_.back().markPaid();
            }
            ReportGenerator report("Расчёт зарплаты", today_);
            std::cout << report.payrollReport(payroll_);
        }
    }

    void reportsMenu() {
        ReportGenerator report("Отчёт по складу", today_);
        std::cout << "1. Остатки\n2. Нехватка\n3. Продажи\n4. Поставщики\n0. Назад\n";
        int c = readInt("> ", 0, 4);
        const Inventory& inventory = system_.getWarehouse().getInventory();
        if (c == 1) std::cout << report.stockReport(inventory, system_.getCatalog());
        if (c == 2) std::cout << report.lowStockReport(inventory, system_.getCatalog());
        if (c == 3) {
            std::cout << report.salesReport(system_.getOrders());
            std::cout << "Выручка (оплаченные): " << system_.revenue().toString() << ", баланс кассы: "
                      << staff_.accountant.getLedgerBalance().toString() << "\n";
        }
        if (c == 4) std::cout << report.supplierReport(system_.getSuppliers());
    }

    void climateMenu() {
        Warehouse& warehouse = system_.getWarehouse();
        for (const auto& z : warehouse.getZones()) {
            std::cout << z.getId() << " " << z.getName() << ": " << z.getClimate().getCurrentTemp() << " °C, "
                      << z.getClimate().getCurrentHumidity() << "% — " << (z.isClimateOk() ? "норма" : "НАРУШЕНИЕ") << "\n";
        }
        std::cout << "1. Задать показания зоны\n0. Назад\n";
        if (readInt("> ", 0, 1) == 1) {
            std::string zoneId = readLine("Зона (Z-A или Z-B): ");
            int t = readInt("Температура (-20..60): ", -20, 60);
            int h = readInt("Влажность (0..100): ", 0, 100);
            Zone& zone = warehouse.findZone(zoneId);
            zone.updateClimate(t, h);
            zone.assertClimate();
            std::cout << "Показания в норме.\n";
        }
    }

    void auditMenu() {
        std::cout << "1. Провести инвентаризацию\n2. Имитировать потерю книги\n0. Назад\n";
        int c = readInt("> ", 0, 2);
        if (c == 2) {
            auto book = chooseBook("Книга, которая «потерялась»");
            if (book) {
                keeperFor(*book).takeBook(zoneFor(*book), book->getIsbn(), 1);
                std::cout << "Одна книга исчезла с полки (в учёте она ещё числится).\n";
            }
        } else if (c == 1) {
            InventoryAudit audit(nextAuditId_++, today_, staff_.manager.getId());
            for (const auto& b : system_.getCatalog().allBooks()) {
                int expected = system_.getWarehouse().getInventory().getItem(b.getIsbn()).getQuantity();
                keeperFor(b).countBook(zoneFor(b), b.getIsbn(), expected, audit);
            }
            staff_.manager.closeAudit(audit);
            if (!audit.hasDiscrepancies()) std::cout << "Расхождений нет.\n";
            for (const auto& d : audit.getDiscrepancies()) {
                const Book& b = system_.getCatalog().findByIsbn(d.getIsbn());
                std::cout << b.getTitle() << ": по учёту " << d.getExpected() << ", по факту " << d.getActual() << ", влияние "
                          << d.valueImpact(b.getPrice()).toString() << (d.isShortage() ? " (недостача)" : " (излишек)") << "\n";
            }
            std::cout << "Итого недостача: " << audit.totalShortage() << ", излишки: " << audit.totalSurplus() << "\n";
        }
    }

    void logMenu() {
        for (const auto& e : system_.getLog().lastEntries(20)) std::cout << e << "\n";
        std::cout << "Всего записей: " << system_.getLog().size() << "\n";
    }

    // ---------------------------------------------------- демонстрации

    static void attempt(const std::string& title, const std::function<void()>& action) {
        try {
            action();
            std::cout << "[выполнено]  " << title << "\n";
        } catch (const WarehouseException& e) {
            std::cout << "[исключение] " << title << " -> " << e.what() << "\n";
        }
    }

    void exceptionsDemo() {
        Inventory& inventory = system_.getWarehouse().getInventory();
        std::vector<Book> books = system_.getCatalog().allBooks();
        const Book& book = books.front();
        attempt("InvalidISBNException: ISBN('123')", [] { ISBN bad("123"); });
        attempt("InvalidDateException: 30 февраля", [] { Date bad(2026, 2, 30); });
        attempt("InvalidDataException: сложение разных валют", [] { Money(100).add(Money(100, "USD")); });
        attempt("BookNotFoundException: неизвестный ISBN", [&] { system_.getCatalog().findByIsbn(ISBN("9780425000014")); });
        attempt("InsufficientStockException: резерв 9999 шт.", [&] { inventory.reserve(book.getIsbn(), 9999); });
        attempt("ShelfOverloadException: полка на 100 г", [&] { Shelf tiny("TINY", 100); tiny.place(book, 1); });
        attempt("ClimateViolationException: +45 °C", [&] {
            Zone& z = system_.getWarehouse().findZone("Z-A");
            z.updateClimate(45.0, 90.0);
            struct Restore {
                Zone& zone;
                ~Restore() { zone.updateClimate(20.0, 45.0); }
            } restore{z};
            z.assertClimate();
        });
        attempt("OrderNotFoundException: заказ №999", [&] { system_.findOrder(999); });
        attempt("InvalidOrderStateException: отгрузка нового заказа", [&] {
            CustomerOrder fresh(0, system_.getCustomers().front(), today_, Address());
            fresh.ship();
        });
        attempt("PaymentFailedException: платёж на 0", [&] { Payment p(0, Money(), PaymentMethod::Card, today_); p.authorize(); });
        attempt("DiscountExpiredException: промокод OLD2020", [&] { findDiscount("OLD2020").applyTo(Money::fromUnits(5000), today_); });
        attempt("SupplierNotFoundException: поставщик S99", [&] { system_.findSupplier("S99"); });
        attempt("AccessDeniedException: кладовщик чужой зоны", [&] {
            staff_.keeperTech.placeBook(system_.getWarehouse().findZone("Z-A"), book, 1);
        });
        attempt("DuplicateEntityException: повторное добавление книги", [&] { system_.getCatalog().addBook(book); });
        attempt("WarehouseException (базовое): ручной вызов", [] { throw WarehouseException("общая ошибка склада"); });
    }

    const Book& bookByTitle(const std::vector<Book>& books, const std::string& title) const {
        for (const auto& b : books) {
            if (b.getTitle() == title) return b;
        }
        throw BookNotFoundException(title);
    }

    void fullDemo() {
        std::cout << "== Сценарий: закупка -> заказ -> оплата -> отгрузка -> возврат ==\n";
        std::vector<Book> books = system_.getCatalog().allBooks();
        const Book& cpp = bookByTitle(books, "Язык программирования C++");
        const Book& crime = bookByTitle(books, "Преступление и наказание");
        const Book& nosov = bookByTitle(books, "Приключения Незнайки");
        restock(cpp, 20);
        int id = placeOrder("C001", {{crime.getIsbn(), 2}, {cpp.getIsbn(), 1}}, "BOOK10");
        printOrder(system_.findOrder(id));
        payAndShip(id, PaymentMethod::Card);
        confirmDelivery(id);
        returnItem(id, crime.getIsbn(), 1, "не подошло");
        std::cout << "\n== Возрастное ограничение ==\n";
        attempt("Ребёнок (10 лет) заказывает «Преступление и наказание» (12+)",
                [&] { placeOrder("C003", {{crime.getIsbn(), 1}}, ""); });
        attempt("Ребёнок (10 лет) заказывает «Приключения Незнайки» (0+)",
                [&] { placeOrder("C003", {{nosov.getIsbn(), 1}}, ""); });
        std::cout << "\n== Исключения ==\n";
        exceptionsDemo();
        std::cout << "\n";
        ReportGenerator report("Итоги", today_);
        std::cout << report.stockReport(system_.getWarehouse().getInventory(), system_.getCatalog());
        std::cout << report.salesReport(system_.getOrders());
    }

    void run() {
        const char* menu =
            "\n=========== КНИЖНЫЙ СКЛАД ===========\n"
            " 1. Каталог книг\n 2. Поиск книги\n 3. Склад: остатки, штрихкод, списание, паллеты\n"
            " 4. Закупка у поставщика\n 5. Новый заказ клиента\n 6. Оплата и отгрузка заказа\n"
            " 7. Подтвердить доставку\n 8. Возврат товара\n 9. Резервы\n10. Персонал, смены, зарплата\n"
            "11. Отчёты\n12. Климат в зонах\n13. Инвентаризация\n14. Журнал действий\n"
            "15. Демо-сценарий целиком\n16. Демонстрация исключений\n 0. Выход\n";
        while (!g_eof) {
            std::cout << menu;
            int choice = readInt("Ваш выбор: ", 0, 16);
            if (g_eof || choice == 0) break;
            try {
                switch (choice) {
                    case 1: showCatalog(); break;
                    case 2: searchBooks(); break;
                    case 3: stockMenu(); break;
                    case 4: procurementMenu(); break;
                    case 5: newOrderMenu(); break;
                    case 6: shipMenu(); break;
                    case 7: deliveryMenu(); break;
                    case 8: returnsMenu(); break;
                    case 9: reservationsMenu(); break;
                    case 10: staffMenu(); break;
                    case 11: reportsMenu(); break;
                    case 12: climateMenu(); break;
                    case 13: auditMenu(); break;
                    case 14: logMenu(); break;
                    case 15: fullDemo(); break;
                    case 16: exceptionsDemo(); break;
                    default: break;
                }
            } catch (const WarehouseException& e) {
                std::cout << "Ошибка: " << e.what() << "\n";
            } catch (const std::exception& e) {
                std::cout << "Непредвиденная ошибка: " << e.what() << "\n";
            }
        }
        std::cout << "До свидания!\n";
    }
};

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    try {
        ConsoleApp app;
        if (argc > 1 && std::string(argv[1]) == "--demo") {
            app.fullDemo();
        } else {
            app.run();
        }
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
