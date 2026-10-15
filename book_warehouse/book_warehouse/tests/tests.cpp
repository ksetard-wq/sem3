// Простые модульные тесты без внешних библиотек.
#include <iostream>
#include <windows.h>
#include "Accountant.h"
#include "BookCatalog.h"
#include "CustomerOrder.h"
#include "Discount.h"
#include "Forklift.h"
#include "Inventory.h"
#include "Loader.h"
#include "ProcurementManager.h"
#include "PurchaseOrder.h"
#include "Schedule.h"
#include "Shelf.h"
#include "Storekeeper.h"
#include "SupplierInvoice.h"
#include "Zone.h"

static int g_failed = 0;
static int g_total = 0;

#define CHECK(cond)                                                                 \
    do {                                                                            \
        ++g_total;                                                                  \
        if (!(cond)) {                                                              \
            ++g_failed;                                                             \
            std::cout << "FAILED: " << #cond << " (строка " << __LINE__ << ")\n";   \
        }                                                                           \
    } while (0)

#define CHECK_THROWS(expr, ExceptionType)                                           \
    do {                                                                            \
        ++g_total;                                                                  \
        bool caught = false;                                                        \
        try {                                                                       \
            expr;                                                                   \
        } catch (const ExceptionType&) {                                            \
            caught = true;                                                          \
        }                                                                           \
        if (!caught) {                                                              \
            ++g_failed;                                                             \
            std::cout << "FAILED: ожидалось " #ExceptionType " (строка " << __LINE__ << ")\n"; \
        }                                                                           \
    } while (0)

namespace {

Publisher publisher() { return Publisher("P", "Издатель", Address("ул. 1", "Москва", "100000", "Россия"), "97851", 1990); }
Author author() { return Author("A", "Автор", "Россия", 1900); }
Genre genre() { return Genre("G", "Классика", 12); }

Book makeBook(const std::string& isbn = "9785170123452", int weight = 500) {
    return Book(ISBN(isbn), "Тестовая книга", author(), publisher(), genre(), Money::fromUnits(100), weight, 100, 2020);
}

Employee makeEmployee(const std::string& id) {
    return Employee(id, "Имя", "Фамилия", ContactInfo("+7 900 123 45 67", "a@b.ru"), Date(1990, 1, 1), "Должность",
                    Money::fromUnits(50000), Date(2020, 1, 1));
}

Customer makeCustomer(int birthYear = 1990) {
    return Customer("C1", "Иван", "Петров", ContactInfo("+7 900 123 45 67", "i@p.ru"), Date(birthYear, 1, 1),
                    Address("ул.", "Город", "123", "Россия"), LoyaltyCard("L1", Date(2026, 1, 1)));
}

void testDateAndMoney() {
    CHECK(Date::isLeapYear(2024));
    CHECK(!Date::isLeapYear(2100));
    CHECK(Date(2026, 3, 1).daysUntil(Date(2026, 3, 31)) == 30);
    CHECK(Date(2024, 2, 28).addDays(2).toString() == "2024-03-01");
    CHECK(Date(2026, 1, 1).isBefore(Date(2026, 1, 2)));
    CHECK_THROWS(Date(2026, 2, 30), InvalidDateException);

    Money a = Money::fromUnits(10.5);
    CHECK(a.toString() == "10.50 RUB");
    CHECK(a.add(Money::fromUnits(1)).getCents() == 1150);
    CHECK(a.multiply(3).getCents() == 3150);
    CHECK(a.percentOf(10).getCents() == 105);
    CHECK(Money(-5).isNegative());
    CHECK_THROWS(a.add(Money(100, "USD")), InvalidDataException);
}

void testIsbnAndContacts() {
    CHECK(ISBN::isValidCode("9785170123452"));
    CHECK(!ISBN::isValidCode("9785170123453"));
    CHECK(ISBN("978-5-17-012345-2").getCode() == "9785170123452");
    CHECK(ISBN("9785170123452").isRussianGroup());
    CHECK_THROWS(ISBN("12345"), InvalidISBNException);
    CHECK(ContactInfo("+7 (900) 123-45-67", "a@b.ru").isPhoneValid());
    CHECK(!ContactInfo("12", "ab").isEmailValid());
    CHECK(ContactInfo("+79001234567", "").maskedPhone().substr(0, 3) == "***");
    CHECK(Person("1", "А", "Б", ContactInfo(), Date(2000, 6, 15)).ageOn(Date(2026, 6, 14)) == 25);
}

void testCatalog() {
    BookCatalog catalog;
    catalog.addBook(makeBook());
    CHECK(catalog.size() == 1);
    CHECK(catalog.search("тестовая").size() == 1);
    CHECK(catalog.search("ТЕСТОВАЯ").size() == 1);
    CHECK(catalog.findByAuthor(author()).size() == 1);
    CHECK_THROWS(catalog.addBook(makeBook()), DuplicateEntityException);
    CHECK_THROWS(catalog.findByIsbn(ISBN("9785517000019")), BookNotFoundException);
    catalog.removeBook(ISBN("9785170123452"));
    CHECK(catalog.size() == 0);
}

void testStockAndShelves() {
    StockItem item(ISBN("9785170123452"), 10, 3, "A-1");
    item.reserve(4);
    CHECK(item.available() == 6);
    CHECK_THROWS(item.issue(7), InsufficientStockException);
    item.releaseReservation(4);
    item.issue(8);
    CHECK(item.needsReorder());

    Shelf shelf("A-1", 1000);
    Book book = makeBook();
    shelf.place(book, 2);
    CHECK(shelf.quantityOf(book.getIsbn()) == 2);
    CHECK(shelf.loadPercent() == 100.0);
    CHECK_THROWS(shelf.place(book, 1), ShelfOverloadException);
    shelf.take(book.getIsbn(), 2);
    CHECK(shelf.isEmpty());

    Zone zone("Z", "Зона");
    zone.addShelf(Shelf("S1", 1000));
    zone.addShelf(Shelf("S2", 1000));
    zone.placeBook(book, 2);
    zone.placeBook(book, 2);
    CHECK(zone.quantityOf(book.getIsbn()) == 4);
    zone.removeBook(book.getIsbn(), 3);
    CHECK(zone.quantityOf(book.getIsbn()) == 1);
    CHECK_THROWS(zone.removeBook(book.getIsbn(), 5), InsufficientStockException);
    zone.updateClimate(40, 20);
    CHECK_THROWS(zone.assertClimate(), ClimateViolationException);
}

void testInventory() {
    Inventory inventory;
    Employee e = makeEmployee("E1");
    Book book = makeBook();
    inventory.registerBook(book.getIsbn(), 2, "A-1");
    CHECK_THROWS(inventory.registerBook(book.getIsbn(), 2, "A-1"), DuplicateEntityException);
    inventory.receive(book.getIsbn(), 10, e, Date(2026, 1, 1));
    inventory.issue(book.getIsbn(), 3, e, Date(2026, 1, 2));
    inventory.writeOff(book.getIsbn(), 1, "повреждена водой", e, Date(2026, 1, 3));
    CHECK(inventory.totalUnits() == 6);
    CHECK(inventory.totalWrittenOff() == 1);
    CHECK(inventory.movementsFor(book.getIsbn()).size() == 3);
    CHECK(inventory.getWriteOffs().front().isDamage());
    CHECK_THROWS(inventory.receive(ISBN("9785517000019"), 1, e, Date(2026, 1, 1)), BookNotFoundException);
}

void testOrders() {
    Discount discount("D10", 10.0, Money::fromUnits(100), Date(2026, 12, 31), 1);
    CustomerOrder order(1, makeCustomer(), Date(2026, 5, 1), Address("ул.", "Город", "123", "Россия"));
    order.addLine(OrderLine(ISBN("9785170123452"), 2, Money::fromUnits(100)));
    order.addLine(OrderLine(ISBN("9785170123452"), 1, Money::fromUnits(100)));
    CHECK(order.totalQuantity() == 3);
    CHECK(order.subtotal().getCents() == 30000);
    order.applyDiscount(discount, Date(2026, 5, 1));
    CHECK(order.total().getCents() == 27000);
    CHECK(discount.remainingUses() == 0);
    CHECK_THROWS(order.ship(), InvalidOrderStateException);
    order.confirm();
    CHECK_THROWS(order.pack(), PaymentFailedException);

    Payment payment(1, order.total(), PaymentMethod::Card, Date(2026, 5, 1));
    CHECK_THROWS(order.pay(payment), PaymentFailedException);
    payment.process();
    order.pay(payment);
    order.pack();
    order.ship();
    order.deliver();
    CHECK(order.getStatus() == OrderStatus::Delivered);
    CHECK_THROWS(order.cancel(), InvalidOrderStateException);
    CHECK_THROWS(Discount("OLD", 5, Money(), Date(2020, 1, 1), 1).applyTo(Money::fromUnits(10), Date(2026, 1, 1)),
                 DiscountExpiredException);
}

void testProcurementAndFinance() {
    Supplier supplier("S1", "Поставщик", Address(), ContactInfo());
    ISBN isbn("9785170123452");
    supplier.addPrice(isbn, Money::fromUnits(60));
    CHECK(supplier.supplies(isbn));
    CHECK_THROWS(supplier.priceFor(ISBN("9785517000019")), BookNotFoundException);
    ProcurementManager buyer(makeEmployee("E2"), Money::fromUnits(1000));
    PurchaseOrder order = buyer.createPurchaseOrder(1, supplier, isbn, 10, Date(2026, 1, 1), Date(2026, 1, 8));
    CHECK(order.totalCost().getCents() == 60000);
    buyer.approveOrder(order);
    CHECK(buyer.getBudget().getCents() == 40000);
    order.markSent();
    CHECK(order.isOverdue(Date(2026, 1, 10)));
    PurchaseOrder big = buyer.createPurchaseOrder(2, supplier, isbn, 100, Date(2026, 1, 1), Date(2026, 1, 8));
    CHECK_THROWS(buyer.approveOrder(big), AccessDeniedException);
    std::vector<Supplier> none;
    CHECK_THROWS(buyer.chooseCheapestSupplier(none, isbn), SupplierNotFoundException);

    SupplierInvoice invoice("INV", 1, Money::fromUnits(600), Date(2026, 1, 1), Date(2026, 1, 15));
    CHECK(invoice.penalty(Date(2026, 1, 20), 1.0).getCents() == 3000);
    Payment payment(1, Money::fromUnits(600), PaymentMethod::BankTransfer, Date(2026, 1, 2));
    payment.process();
    Accountant accountant(makeEmployee("E3"), Money::fromUnits(1000));
    accountant.payInvoice(invoice, payment);
    CHECK(accountant.getLedgerBalance().getCents() == 40000);
    CHECK_THROWS(accountant.payInvoice(invoice, payment), InvalidOrderStateException);
    TaxCalculator tax;
    CHECK(tax.incomeTax(Money::fromUnits(1000)).getCents() == 13000);
    CHECK(tax.vatFor(Money::fromUnits(100), true).getCents() == 1000);
    PayrollRecord payroll = accountant.buildPayroll(makeEmployee("E4"), "2026-01", Money(), tax);
    CHECK(payroll.netPay().getCents() == 5000000 - 650000);
}

void testStaff() {
    Zone zoneA("Z-A", "A");
    zoneA.addShelf(Shelf("S", 100000));
    Zone zoneB("Z-B", "B");
    Storekeeper keeper(makeEmployee("E5"), "Z-A", BarcodeScanner("SC"));
    Book book = makeBook();
    CHECK(keeper.placeBook(zoneA, book, 2) == "S");
    CHECK_THROWS(keeper.placeBook(zoneB, book, 1), AccessDeniedException);
    InventoryAudit audit(1, Date(2026, 1, 1), "E1");
    keeper.countBook(zoneA, book.getIsbn(), 5, audit);
    CHECK(audit.hasDiscrepancies());
    CHECK(audit.totalShortage() == 3);
    audit.close();
    CHECK_THROWS(audit.recordCount(book.getIsbn(), 1, 2), InvalidOrderStateException);

    Loader loader(makeEmployee("E6"), 5);
    Pallet pallet("P", 100000);
    CHECK_THROWS(loader.loadPallet(pallet, makeBook("9785170123452", 3000), 2), AccessDeniedException);
    loader.certifyForklift();
    loader.loadPallet(pallet, book, 4);
    Forklift forklift("F", 1);
    CHECK_THROWS(forklift.liftPallet(pallet), InvalidOrderStateException);
    forklift.assignOperator(loader);
    CHECK_THROWS(forklift.liftPallet(pallet), ShelfOverloadException);

    Schedule schedule(Date(2026, 1, 1));
    schedule.addShift(Shift("E1", Date(2026, 1, 1), 8, 16));
    CHECK_THROWS(schedule.addShift(Shift("E1", Date(2026, 1, 1), 15, 20)), DuplicateEntityException);
    schedule.addShift(Shift("E2", Date(2026, 1, 1), 15, 20));
    CHECK(schedule.hoursFor("E1") == 8);
    CHECK(schedule.isEmployeeBusy("E1", Date(2026, 1, 1), 10));
    CHECK_THROWS(Shift("E1", Date(2026, 1, 1), 10, 10), InvalidDataException);

    BarcodeScanner scanner("SC");
    CHECK(scanner.scan("978-5-17-012345-2").getCode() == "9785170123452");
    CHECK_THROWS(scanner.scan("abc"), InvalidISBNException);
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
    testDateAndMoney();
    testIsbnAndContacts();
    testCatalog();
    testStockAndShelves();
    testInventory();
    testOrders();
    testProcurementAndFinance();
    testStaff();
    std::cout << "Проверок: " << g_total << ", провалено: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
