# Лабораторная работа №2. Крупный ООП-проект

Предметная область: **книжный склад**. Модель описывает каталог книг (авторы, издательства, жанры), складскую структуру (зоны, стеллажи, паллеты, погрузочные доки, климат-контроль), складской учёт и инвентаризацию, закупки у поставщиков, заказы клиентов, оплату, доставку и возвраты, а также персонал (заведующий, кладовщики, комплектовщик, грузчик, бухгалтер, менеджер по закупкам), графики смен и зарплаты.

Язык: **C++17**. Все классы лежат в заголовочных файлах `include/*.h` (по одному классу на файл), консольное приложение и сценарии работы с классами — в `src/main.cpp`.

## Структура репозитория

```
include/      классы предметной области (.h) и exceptions.h
src/main.cpp  консольное меню и демо-сценарии
tests/        модульные тесты (tests/tests.cpp)
tools/        gen_readme.py — генератор этого README
CMakeLists.txt
```

## Сборка и запуск

```bash
cmake -S . -B build
cmake --build build
./build/book_warehouse          # интерактивное меню
./build/book_warehouse --demo   # автоматический демо-сценарий
./build/run_tests              # тесты
```

Без CMake: `g++ -std=c++17 -Iinclude src/main.cpp -o book_warehouse`. В Windows консоль переключается в UTF-8 автоматически.

## Консольное меню

Каталог и поиск книг; остатки, штрихкод, списание, погрузка паллет; закупка у поставщика; оформление заказа клиента (с промокодом и проверкой возрастного ограничения); оплата и отгрузка; подтверждение доставки; возврат; резервы; персонал, смены и зарплата; отчёты; климат в зонах; инвентаризация; журнал действий; демо-сценарий целиком; демонстрация всех исключений.

## Классы

Формат строки: `Класс | число полей | число методов (поведений) | связанные классы`. Геттеры, конструкторы и деструкторы в число методов не входят.

| Класс | Поля | Методы | Ассоциации (связанные классы) |
| ----- | ---- | ------ | ----------------------------- |
| Accountant | 2 | 6 | Employee, Money, Payment, SupplierInvoice, TaxCalculator |
| ActivityLog | 2 | 5 | Date |
| Address | 4 | 4 | — |
| AuditDiscrepancy | 3 | 4 | ISBN, Money |
| Author | 5 | 5 | ISBN |
| BarcodeScanner | 4 | 5 | ISBN |
| Book | 9 | 6 | Author, Genre, ISBN, Money, Publisher |
| BookCatalog | 2 | 11 | Author, Book, Genre, ISBN, Publisher |
| ClimateControl | 6 | 6 | — |
| ContactInfo | 2 | 4 | — |
| Customer | 4 | 8 | Address, ContactInfo, Date, LoyaltyCard, Money, Person |
| CustomerOrder | 8 | 14 | Address, Customer, Date, Discount, ISBN, OrderLine, Payment |
| Date | 3 | 9 | — |
| Discount | 6 | 5 | Date, Money |
| Employee | 4 | 6 | ContactInfo, Date, Money, Person |
| Forklift | 5 | 5 | Loader, Pallet |
| Genre | 3 | 3 | — |
| ISBN | 1 | 7 | — |
| InboundShipment | 7 | 7 | Date, ISBN, LoadingDock, PurchaseOrder, Storekeeper |
| Inventory | 4 | 13 | Date, Employee, ISBN, StockItem, StockMovement, WriteOffRecord |
| InventoryAudit | 5 | 6 | AuditDiscrepancy, Date, ISBN |
| Loader | 2 | 5 | Book, Employee, Pallet |
| LoadingDock | 4 | 4 | Date |
| LoyaltyCard | 4 | 4 | Date, Money |
| Money | 2 | 10 | — |
| OrderLine | 3 | 4 | ISBN, Money |
| OutboundShipment | 7 | 7 | Address, Date, Package, ShippingCarrier |
| Package | 5 | 5 | Book |
| Pallet | 4 | 4 | Book |
| Payment | 5 | 7 | Date, Money |
| PayrollRecord | 6 | 5 | Money |
| Person | 5 | 4 | ContactInfo, Date |
| Picker | 2 | 5 | BookCatalog, CustomerOrder, Employee, Package |
| PricingPolicy | 3 | 5 | Money |
| ProcurementManager | 2 | 6 | Date, Employee, ISBN, Money, PurchaseOrder, Supplier |
| Publisher | 5 | 3 | Address, ISBN |
| PurchaseOrder | 6 | 7 | Date, OrderLine, Supplier |
| ReportGenerator | 2 | 6 | BookCatalog, CustomerOrder, Date, Inventory, PayrollRecord, Supplier |
| Reservation | 6 | 4 | Date, ISBN |
| ReturnRequest | 7 | 5 | Date, ISBN, Money |
| Schedule | 2 | 5 | Date, Shift |
| Shelf | 5 | 7 | Book, ISBN |
| Shift | 4 | 4 | Date |
| ShippingCarrier | 5 | 4 | Date, Money, Package |
| StockItem | 5 | 8 | ISBN |
| StockMovement | 6 | 4 | Date, ISBN |
| Storekeeper | 2 | 7 | BarcodeScanner, Book, Employee, ISBN, InventoryAudit, Zone |
| Supplier | 7 | 7 | Address, ContactInfo, ISBN, Money |
| SupplierInvoice | 6 | 4 | Date, Money, Payment |
| TaxCalculator | 3 | 4 | Money |
| Warehouse | 7 | 9 | Address, Date, ISBN, Inventory, LoadingDock, WarehouseManager, Zone |
| WarehouseManager | 2 | 7 | Employee, InventoryAudit, Money, Schedule, Shift, WriteOffRecord |
| WarehouseSystem | 10 | 17 | ActivityLog, Book, BookCatalog, Customer, CustomerOrder, Date, Employee, ISBN, PricingPolicy, PurchaseOrder, Supplier, Warehouse |
| WriteOffRecord | 6 | 3 | Date, ISBN, Money |
| Zone | 4 | 10 | Book, ClimateControl, ISBN, Shelf |
| Exceptions | 3 | 0 | InvalidISBNException, InvalidDateException, InvalidDataException и др. |

### Исключения (14)

- InvalidISBNException (наследник WarehouseException)
- InvalidDateException (наследник WarehouseException)
- InvalidDataException (наследник WarehouseException)
- BookNotFoundException (наследник WarehouseException)
- InsufficientStockException (наследник WarehouseException)
- ShelfOverloadException (наследник WarehouseException)
- ClimateViolationException (наследник WarehouseException)
- OrderNotFoundException (наследник WarehouseException)
- InvalidOrderStateException (наследник WarehouseException)
- PaymentFailedException (наследник WarehouseException)
- DiscountExpiredException (наследник WarehouseException)
- SupplierNotFoundException (наследник WarehouseException)
- AccessDeniedException (наследник WarehouseException)
- DuplicateEntityException (наследник WarehouseException)

### Итоговая статистика

| Показатель | Значение |
| ---------- | -------- |
| Классы предметной области | 55 |
| Классы исключений | 14 |
| Поля | 243 |
| Поведения | 339 |
| Ассоциации | 158 |
| Исключения | 14 |

Требования задания: ≥ 50 классов, ≥ 150 полей, ≥ 100 поведений, ≥ 30 ассоциаций, ≥ 12 исключений. Поля исключений (3 шт.) в общее число не входят. Поведения подсчитаны построчно по каждому классу, перегруженные методы с одним именем считаются один раз.

## Подробное описание классов

### Accountant

Бухгалтер: оплата счетов, ведение кассы, расчёт зарплат. Наследует `Employee`.

**Поля:** `ledgerBalance_` — `Money`; `processedInvoices_` — `int`

**Методы:** `buildPayroll`, `isInDeficit`, `payInvoice`, `recordExpense`, `recordIncome`, `role`

**Ассоциации:** Employee, Money, Payment, SupplierInvoice, TaxCalculator

### ActivityLog

Журнал действий системы с ограничением размера.

**Поля:** `entries_` — `std::vector<std::string>`; `capacity_` — `size_t`

**Методы:** `clear`, `entriesBy`, `lastEntries`, `log`, `size`

**Ассоциации:** Date

### Address

Почтовый адрес.

**Поля:** `street_` — `std::string`; `city_` — `std::string`; `postalCode_` — `std::string`; `country_` — `std::string`

**Методы:** `fullAddress`, `isComplete`, `isDomestic`, `isSameCity`

**Ассоциации:** —

### AuditDiscrepancy

Расхождение между учётным и фактическим количеством книги.

**Поля:** `isbn_` — `ISBN`; `expected_` — `int`; `actual_` — `int`

**Методы:** `difference`, `isShortage`, `isSurplus`, `valueImpact`

**Ассоциации:** ISBN, Money

### Author

Автор книг.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `country_` — `std::string`; `birthYear_` — `int`; `works_` — `std::vector<std::string>`

**Методы:** `addWork`, `ageIn`, `hasWritten`, `isContemporary`, `worksCount`

**Ассоциации:** ISBN

### BarcodeScanner

Сканер штрихкодов (ISBN-13) с зарядом батареи.

**Поля:** `id_` — `std::string`; `lastScanned_` — `std::string`; `batteryPercent_` — `int`; `scanCount_` — `int`

**Методы:** `isBatteryLow`, `recharge`, `resetCounter`, `scan`, `verify`

**Ассоциации:** ISBN

### Book

Книга: ISBN, автор, издательство, жанр, цена, вес.

**Поля:** `isbn_` — `ISBN`; `title_` — `std::string`; `author_` — `Author`; `publisher_` — `Publisher`; `genre_` — `Genre`; `price_` — `Money`; `weightGrams_` — `int`; `pages_` — `int`; `publicationYear_` — `int`

**Методы:** `changePrice`, `discountedPrice`, `isOldEdition`, `isSuitableForAge`, `matchesQuery`, `weightKg`

**Ассоциации:** Author, Genre, ISBN, Money, Publisher

### BookCatalog

Каталог книг: добавление, удаление, поиск.

**Поля:** `books_` — `std::map<std::string, Book>`; `genres_` — `std::vector<Genre>`

**Методы:** `addBook`, `allBooks`, `contains`, `findByAuthor`, `findByGenre`, `findByIsbn`, `findByPublisher`, `registerGenre`, `removeBook`, `search`, `size`

**Ассоциации:** Author, Book, Genre, ISBN, Publisher

### ClimateControl

Климат-контроль зоны: температура и влажность.

**Поля:** `minTemp_` — `double`; `maxTemp_` — `double`; `minHumidity_` — `double`; `maxHumidity_` — `double`; `currentTemp_` — `double`; `currentHumidity_` — `double`

**Методы:** `adjustLimits`, `assertWithinRange`, `isHumidityOk`, `isTemperatureOk`, `isWithinRange`, `updateReadings`

**Ассоциации:** —

### ContactInfo

Телефон и e-mail с проверкой формата.

**Поля:** `phone_` — `std::string`; `email_` — `std::string`

**Методы:** `hasAnyContact`, `isEmailValid`, `isPhoneValid`, `maskedPhone`

**Ассоциации:** —

### Customer

Клиент магазина (наследник Person).

**Поля:** `address_` — `Address`; `loyaltyCard_` — `LoyaltyCard`; `blocked_` — `bool`; `orderIds_` — `std::vector<int>`

**Методы:** `block`, `canOrder`, `earnPoints`, `isVip`, `orderCount`, `registerOrder`, `unblock`, `updateAddress`

**Ассоциации:** Address, ContactInfo, Date, LoyaltyCard, Money, Person

### CustomerOrder

Заказ клиента со статусами, скидкой и оплатой.

**Поля:** `id_` — `int`; `customer_` — `Customer`; `lines_` — `std::vector<OrderLine>`; `status_` — `OrderStatus`; `createdAt_` — `Date`; `shippingAddress_` — `Address`; `discountPercent_` — `double`; `payments_` — `std::vector<Payment>`

**Методы:** `addLine`, `applyDiscount`, `cancel`, `confirm`, `deliver`, `isPaid`, `pack`, `pay`, `removeLine`, `requireStatus`, `ship`, `subtotal`, `total`, `totalQuantity`

**Ассоциации:** Address, Customer, Date, Discount, ISBN, OrderLine, Payment

### Date

Календарная дата с арифметикой.

**Поля:** `year_` — `int`; `month_` — `int`; `day_` — `int`

**Методы:** `addDays`, `daysInMonth`, `daysUntil`, `fromDayNumber`, `isBefore`, `isLeapYear`, `isValidDate`, `toDayNumber`, `toString`

**Ассоциации:** —

### Discount

Промокод со сроком действия и лимитом использований.

**Поля:** `code_` — `std::string`; `percent_` — `double`; `minAmount_` — `Money`; `validUntil_` — `Date`; `usageLimit_` — `int`; `used_` — `int`

**Методы:** `applyTo`, `isApplicable`, `isValidOn`, `registerUse`, `remainingUses`

**Ассоциации:** Date, Money

### Employee

Сотрудник склада (наследник Person).

**Поля:** `position_` — `std::string`; `salary_` — `Money`; `hireDate_` — `Date`; `active_` — `bool`

**Методы:** `canApproveOrders`, `dismiss`, `isActive`, `raiseSalary`, `role`, `yearsOfService`

**Ассоциации:** ContactInfo, Date, Money, Person

### Forklift

Погрузчик: оператор, грузоподъёмность, заряд.

**Поля:** `id_` — `std::string`; `capacityKg_` — `int`; `operatorId_` — `std::string`; `batteryPercent_` — `int`; `inUse_` — `bool`

**Методы:** `assignOperator`, `canLift`, `liftPallet`, `recharge`, `releaseOperator`

**Ассоциации:** Loader, Pallet

### Genre

Жанр с возрастным ограничением.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `minAge_` — `int`

**Методы:** `isChildrenGenre`, `isSuitableForAge`, `matchesName`

**Ассоциации:** —

### ISBN

Код ISBN-13 с проверкой контрольной цифры.

**Поля:** `code_` — `std::string`

**Методы:** `computeCheckDigit`, `equals`, `formatted`, `hasPrefix`, `isRussianGroup`, `isValidCode`, `normalize`

**Ассоциации:** —

### InboundShipment

Входящая поставка: приёмка по закупке, док, инспекция.

**Поля:** `id_` — `int`; `purchaseOrder_` — `PurchaseOrder`; `arrivalDate_` — `Date`; `received_` — `std::map<std::string, int>`; `dockId_` — `std::string`; `inspected_` — `bool`; `inspectedBy_` — `std::string`

**Методы:** `assignDock`, `inspect`, `isComplete`, `isDelayed`, `missingLines`, `registerReceived`, `totalReceivedUnits`

**Ассоциации:** Date, ISBN, LoadingDock, PurchaseOrder, Storekeeper

### Inventory

Складской учёт: остатки, движения, списания.

**Поля:** `items_` — `std::map<std::string, StockItem>`; `movements_` — `std::vector<StockMovement>`; `writeOffs_` — `std::vector<WriteOffRecord>`; `nextMovementId_` — `int`

**Методы:** `availableQuantity`, `findItem`, `hasBook`, `issue`, `lowStockItems`, `movementsFor`, `receive`, `registerBook`, `releaseReservation`, `reserve`, `totalUnits`, `totalWrittenOff`, `writeOff`

**Ассоциации:** Date, Employee, ISBN, StockItem, StockMovement, WriteOffRecord

### InventoryAudit

Инвентаризация и фиксация расхождений.

**Поля:** `id_` — `int`; `date_` — `Date`; `auditorId_` — `std::string`; `discrepancies_` — `std::vector<AuditDiscrepancy>`; `closed_` — `bool`

**Методы:** `close`, `hasDiscrepancies`, `isClosed`, `recordCount`, `totalShortage`, `totalSurplus`

**Ассоциации:** AuditDiscrepancy, Date, ISBN

### Loader

Грузчик (наследник Employee).

**Поля:** `maxLiftKg_` — `int`; `forkliftCertified_` — `bool`

**Методы:** `canLift`, `certifyForklift`, `isForkliftCertified`, `loadPallet`, `role`

**Ассоциации:** Book, Employee, Pallet

### LoadingDock

Погрузочный док.

**Поля:** `id_` — `std::string`; `occupied_` — `bool`; `currentCarrier_` — `std::string`; `occupiedSince_` — `Date`

**Методы:** `isFree`, `isOccupiedBy`, `occupy`, `release`

**Ассоциации:** Date

### LoyaltyCard

Карта лояльности с баллами и уровнями.

**Поля:** `number_` — `std::string`; `points_` — `int`; `tier_` — `LoyaltyTier`; `issuedAt_` — `Date`

**Методы:** `addPoints`, `discountPercent`, `recalculateTier`, `spendPoints`

**Ассоциации:** Date, Money

### Money

Денежная сумма в копейках с валютой.

**Поля:** `cents_` — `long long`; `currency_` — `std::string`

**Методы:** `add`, `fromUnits`, `isGreaterThan`, `isNegative`, `isZero`, `multiply`, `percentOf`, `requireSameCurrency`, `subtract`, `toString`

**Ассоциации:** —

### OrderLine

Позиция заказа: книга, количество, цена.

**Поля:** `isbn_` — `ISBN`; `quantity_` — `int`; `unitPrice_` — `Money`

**Методы:** `applyDiscount`, `changeQuantity`, `isValid`, `total`

**Ассоциации:** ISBN, Money

### OutboundShipment

Исходящая отправка заказа перевозчиком.

**Поля:** `id_` — `int`; `package_` — `Package`; `carrier_` — `ShippingCarrier`; `trackingNumber_` — `std::string`; `status_` — `ShipmentStatus`; `shippedAt_` — `Date`; `destination_` — `Address`

**Методы:** `cost`, `dispatch`, `eta`, `generateTrackingNumber`, `isDelivered`, `markDelivered`, `markReturned`

**Ассоциации:** Address, Date, Package, ShippingCarrier

### Package

Посылка клиенту.

**Поля:** `id_` — `int`; `orderId_` — `int`; `items_` — `std::map<std::string, int>`; `weightGrams_` — `int`; `sealed_` — `bool`

**Методы:** `addItem`, `isSealed`, `seal`, `unseal`, `weightKg`

**Ассоциации:** Book

### Pallet

Паллета с ограничением по весу.

**Поля:** `id_` — `std::string`; `items_` — `std::map<std::string, int>`; `weightGrams_` — `int`; `maxWeightGrams_` — `int`

**Методы:** `bookCount`, `canLoad`, `load`, `unloadAll`

**Ассоциации:** Book

### Payment

Платёж с жизненным циклом статусов.

**Поля:** `id_` — `int`; `amount_` — `Money`; `method_` — `PaymentMethod`; `status_` — `PaymentStatus`; `date_` — `Date`

**Методы:** `authorize`, `complete`, `fail`, `isSuccessful`, `process`, `processingFee`, `refund`

**Ассоциации:** Date, Money

### PayrollRecord

Расчётный листок сотрудника.

**Поля:** `employeeId_` — `std::string`; `period_` — `std::string`; `baseSalary_` — `Money`; `bonus_` — `Money`; `deductions_` — `Money`; `paid_` — `bool`

**Методы:** `addDeduction`, `applyBonus`, `gross`, `markPaid`, `netPay`

**Ассоциации:** Money

### Person

Базовый класс человека.

**Поля:** `id_` — `std::string`; `firstName_` — `std::string`; `lastName_` — `std::string`; `contact_` — `ContactInfo`; `birthDate_` — `Date`

**Методы:** `ageOn`, `fullName`, `isAdult`, `updateContact`

**Ассоциации:** ContactInfo, Date

### Picker

Комплектовщик заказов (наследник Employee).

**Поля:** `pickedToday_` — `int`; `pickRatePerHour_` — `int`

**Методы:** `estimateHours`, `isOverloaded`, `pickOrder`, `resetDay`, `role`

**Ассоциации:** BookCatalog, CustomerOrder, Employee, Package

### PricingPolicy

Политика ценообразования: наценка, маржа, скидки.

**Поля:** `markupPercent_` — `double`; `minMarginPercent_` — `double`; `seasonalDiscountPercent_` — `double`

**Методы:** `applySeasonal`, `bulkDiscountPercent`, `isMarginOk`, `retailPrice`, `startSeasonalSale`

**Ассоциации:** Money

### ProcurementManager

Менеджер по закупкам (наследник Employee).

**Поля:** `budget_` — `Money`; `approvedCount_` — `int`

**Методы:** `approveOrder`, `canApproveOrders`, `chooseCheapestSupplier`, `createPurchaseOrder`, `replenishBudget`, `role`

**Ассоциации:** Date, Employee, ISBN, Money, PurchaseOrder, Supplier

### Publisher

Издательство.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `address_` — `Address`; `isbnPrefix_` — `std::string`; `foundedYear_` — `int`

**Методы:** `ownsIsbn`, `relocate`, `yearsActive`

**Ассоциации:** Address, ISBN

### PurchaseOrder

Заказ поставщику.

**Поля:** `id_` — `int`; `supplier_` — `Supplier`; `lines_` — `std::vector<OrderLine>`; `status_` — `PurchaseStatus`; `createdAt_` — `Date`; `expectedAt_` — `Date`

**Методы:** `addLine`, `approve`, `cancel`, `isOverdue`, `markReceived`, `markSent`, `totalCost`

**Ассоциации:** Date, OrderLine, Supplier

### ReportGenerator

Текстовые отчёты по складу, продажам, зарплатам.

**Поля:** `title_` — `std::string`; `generatedAt_` — `Date`

**Методы:** `header`, `lowStockReport`, `payrollReport`, `salesReport`, `stockReport`, `supplierReport`

**Ассоциации:** BookCatalog, CustomerOrder, Date, Inventory, PayrollRecord, Supplier

### Reservation

Резерв книги за клиентом на срок.

**Поля:** `id_` — `int`; `customerId_` — `std::string`; `isbn_` — `ISBN`; `quantity_` — `int`; `expiresAt_` — `Date`; `active_` — `bool`

**Методы:** `cancel`, `extend`, `isExpired`, `matches`

**Ассоциации:** Date, ISBN

### ReturnRequest

Заявка на возврат книги.

**Поля:** `id_` — `int`; `orderId_` — `int`; `isbn_` — `ISBN`; `quantity_` — `int`; `reason_` — `std::string`; `status_` — `ReturnStatus`; `requestedAt_` — `Date`

**Методы:** `approve`, `complete`, `isWithinReturnWindow`, `refundAmount`, `reject`

**Ассоциации:** Date, ISBN, Money

### Schedule

График смен сотрудников.

**Поля:** `shifts_` — `std::vector<Shift>`; `weekStart_` — `Date`

**Методы:** `addShift`, `hoursFor`, `isEmployeeBusy`, `removeShiftsFor`, `shiftsFor`

**Ассоциации:** Date, Shift

### Shelf

Стеллаж с ограничением по весу.

**Поля:** `id_` — `std::string`; `maxWeightGrams_` — `int`; `currentWeightGrams_` — `int`; `contents_` — `std::map<std::string, int>`; `unitWeights_` — `std::map<std::string, int>`

**Методы:** `canHold`, `freeWeightGrams`, `isEmpty`, `loadPercent`, `place`, `quantityOf`, `take`

**Ассоциации:** Book, ISBN

### Shift

Рабочая смена сотрудника.

**Поля:** `employeeId_` — `std::string`; `date_` — `Date`; `startHour_` — `int`; `endHour_` — `int`

**Методы:** `coversHour`, `durationHours`, `isNightShift`, `overlaps`

**Ассоциации:** Date

### ShippingCarrier

Перевозчик с тарифами.

**Поля:** `name_` — `std::string`; `baseFee_` — `Money`; `ratePerKg_` — `Money`; `maxWeightGrams_` — `int`; `avgDays_` — `int`

**Методы:** `calculateCost`, `canCarry`, `estimateDelivery`, `isFasterThan`

**Ассоциации:** Date, Money, Package

### StockItem

Складская позиция: количество, резерв, минимальный остаток.

**Поля:** `isbn_` — `ISBN`; `quantity_` — `int`; `reserved_` — `int`; `minLevel_` — `int`; `shelfId_` — `std::string`

**Методы:** `available`, `issue`, `moveToShelf`, `needsReorder`, `receive`, `releaseReservation`, `requirePositive`, `reserve`

**Ассоциации:** ISBN

### StockMovement

Движение товара: приход, расход, списание.

**Поля:** `id_` — `int`; `type_` — `MovementType`; `isbn_` — `ISBN`; `quantity_` — `int`; `date_` — `Date`; `employeeId_` — `std::string`

**Методы:** `describe`, `isInbound`, `isOutbound`, `signedQuantity`

**Ассоциации:** Date, ISBN

### Storekeeper

Кладовщик (наследник Employee), работает в своей зоне.

**Поля:** `assignedZoneId_` — `std::string`; `scanner_` — `BarcodeScanner`

**Методы:** `countBook`, `equipScanner`, `placeBook`, `requireZone`, `role`, `takeBook`, `verifyBook`

**Ассоциации:** BarcodeScanner, Book, Employee, ISBN, InventoryAudit, Zone

### Supplier

Поставщик с прайс-листом и рейтингом.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `address_` — `Address`; `contact_` — `ContactInfo`; `priceList_` — `std::map<std::string, Money>`; `rating_` — `double`; `active_` — `bool`

**Методы:** `activate`, `addPrice`, `deactivate`, `isReliable`, `priceFor`, `rate`, `supplies`

**Ассоциации:** Address, ContactInfo, ISBN, Money

### SupplierInvoice

Счёт поставщика с расчётом пени.

**Поля:** `id_` — `std::string`; `purchaseOrderId_` — `int`; `amount_` — `Money`; `issueDate_` — `Date`; `dueDate_` — `Date`; `paid_` — `bool`

**Методы:** `daysUntilDue`, `isOverdue`, `penalty`, `settle`

**Ассоциации:** Date, Money, Payment

### TaxCalculator

НДС и подоходный налог.

**Поля:** `standardVat_` — `double`; `reducedVat_` — `double`; `incomeTaxPercent_` — `double`

**Методы:** `incomeTax`, `priceWithVat`, `vatFor`, `vatIncluded`

**Ассоциации:** Money

### Warehouse

Склад: зоны, доки, инвентарь.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `address_` — `Address`; `zones_` — `std::vector<Zone>`; `inventory_` — `Inventory`; `docks_` — `std::vector<LoadingDock>`; `managerId_` — `std::string`

**Методы:** `acquireFreeDock`, `addDock`, `addZone`, `assignManager`, `findZone`, `freeDocksCount`, `locateBook`, `totalShelves`, `zonesWithClimateViolation`

**Ассоциации:** Address, Date, ISBN, Inventory, LoadingDock, WarehouseManager, Zone

### WarehouseManager

Заведующий складом (наследник Employee).

**Поля:** `warehouseId_` — `std::string`; `approvalLimit_` — `Money`

**Методы:** `approveWriteOff`, `assignShift`, `canApprove`, `canApproveOrders`, `closeAudit`, `raiseApprovalLimit`, `role`

**Ассоциации:** Employee, InventoryAudit, Money, Schedule, Shift, WriteOffRecord

### WarehouseSystem

Фасад: связывает каталог, склад, клиентов, заказы, закупки.

**Поля:** `catalog_` — `BookCatalog`; `warehouse_` — `Warehouse`; `suppliers_` — `std::vector<Supplier>`; `customers_` — `std::vector<Customer>`; `orders_` — `std::vector<CustomerOrder>`; `purchaseOrders_` — `std::vector<PurchaseOrder>`; `log_` — `ActivityLog`; `pricing_` — `PricingPolicy`; `nextOrderId_` — `int`; `nextPurchaseId_` — `int`

**Методы:** `addBook`, `addSupplier`, `addToOrder`, `cancelOrder`, `confirmOrder`, `createOrder`, `createPurchase`, `findCustomer`, `findOrder`, `findPurchase`, `findSupplier`, `fulfillOrder`, `payOrder`, `receivePurchase`, `registerCustomer`, `revenue`, `shipOrder`

**Ассоциации:** ActivityLog, Book, BookCatalog, Customer, CustomerOrder, Date, Employee, ISBN, PricingPolicy, PurchaseOrder, Supplier, Warehouse

### WriteOffRecord

Запись о списании книг.

**Поля:** `id_` — `int`; `isbn_` — `ISBN`; `quantity_` — `int`; `reason_` — `std::string`; `date_` — `Date`; `approvedBy_` — `std::string`

**Методы:** `describe`, `isDamage`, `lossAmount`

**Ассоциации:** Date, ISBN, Money

### Zone

Зона склада со стеллажами и климатом.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `shelves_` — `std::vector<Shelf>`; `climate_` — `ClimateControl`

**Методы:** `addShelf`, `assertClimate`, `findShelfFor`, `isClimateOk`, `locate`, `placeBook`, `quantityOf`, `removeBook`, `totalCapacityGrams`, `updateClimate`

**Ассоциации:** Book, ClimateControl, ISBN, Shelf
