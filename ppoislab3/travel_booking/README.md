# Лабораторная работа №3. Система бронирования путешествий

Предметная область: **система бронирования путешествий** (турагентство). Модель описывает путешественников с паспортами и визами, учётные записи с проверкой пароля и блокировкой, географию (страны, города, аэропорты), авиарейсы с местами и классами обслуживания, отели с номерами, туры и экскурсии, аренду автомобилей, бронирования разных типов (с полиморфными условиями отмены), платежи, промокоды, бонусную программу, страхование, поддержку клиентов, персонал агентства и аналитику.

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
./build/travel_booking          # интерактивное меню
./build/travel_booking --demo   # автоматический демо-сценарий
./build/run_tests             # тесты
```

Без CMake: `g++ -std=c++17 -Iinclude src/main.cpp -o travel_booking`. В Windows консоль переключается в UTF-8 автоматически.

## Консольное меню

Поиск рейсов, отелей и туров; вход в аккаунт (демо-пользователи `ivan`/`Ivan2026pass`, `maria`/`Maria2026pass`, `oleg`/`Oleg2026pass`); бронирование перелёта, отеля, тура, автомобиля и экскурсии; маршрут с поиском пересечений; оплата с промокодом (`WELCOME10`) и бонусами; отмена со штрафом и возвратом; страхование; поддержка; отзывы; уведомления; конвертер валют; аналитика; демо-сценарий и демонстрация всех исключений.

## Классы

Формат строки: `Класс | число полей | число методов (поведений) | связанные классы`. Геттеры, конструкторы и деструкторы в число методов не входят.

| Класс | Поля | Методы | Ассоциации (связанные классы) |
| ----- | ---- | ------ | ----------------------------- |
| Address | 4 | 4 | — |
| AgencyManager | 2 | 6 | Money, Staff, UserAccount |
| Aircraft | 4 | 4 | — |
| Airline | 4 | 3 | Country |
| Airport | 4 | 4 | City |
| Attraction | 6 | 4 | City, Money |
| Booking | 5 | 14 | Date, Money |
| BookingManager | 3 | 9 | Booking, Date, Money |
| CarRental | 6 | 5 | City, DateRange, Money |
| CarRentalBooking | 4 | 6 | Booking, Date, DateRange, Money |
| City | 5 | 4 | Country, GeoPoint |
| ContactInfo | 2 | 4 | — |
| Country | 4 | 4 | — |
| CurrencyConverter | 2 | 4 | Money |
| Date | 3 | 11 | — |
| DateRange | 2 | 6 | Date |
| Excursion | 8 | 6 | Attraction, Date, Guide, Money |
| ExcursionBooking | 4 | 4 | Booking, Date, Money |
| Flight | 10 | 14 | Aircraft, Airline, Airport, Date, Money, Seat |
| FlightBooking | 6 | 8 | Booking, Date, Money |
| GeoPoint | 2 | 4 | — |
| Guide | 3 | 6 | Attraction, Money, Staff |
| Hotel | 8 | 10 | Address, City, DateRange, Room |
| HotelBooking | 6 | 8 | Booking, Date, DateRange, Money |
| InsuranceClaim | 5 | 4 | Date, Money |
| InsurancePolicy | 7 | 7 | Date, DateRange, InsuranceClaim, Money |
| Itinerary | 5 | 8 | Booking, Date |
| LoyaltyAccount | 4 | 5 | Date, Money |
| Money | 2 | 11 | — |
| Notification | 5 | 4 | Date |
| NotificationService | 3 | 6 | Booking, Date, Notification |
| Passport | 4 | 5 | Country, Date, DateRange |
| Payment | 7 | 7 | Date, Money |
| PaymentProcessor | 4 | 8 | Booking, Date, Money, Payment |
| Person | 5 | 4 | ContactInfo, Date |
| PriceCalculator | 4 | 5 | Date, DateRange, Money |
| PromoCode | 6 | 5 | Date, DateRange, Money |
| RefundCalculator | 3 | 4 | Booking, Date, Money |
| Review | 6 | 4 | Date |
| ReviewBoard | 2 | 5 | Date, Review |
| Room | 5 | 6 | DateRange, Money |
| Seat | 4 | 5 | — |
| Staff | 3 | 4 | ContactInfo, Date, Money, Person |
| SupportOperator | 3 | 6 | Staff, SupportTicket |
| SupportTicket | 8 | 8 | Date |
| TourBooking | 5 | 8 | Booking, Date, DateRange, Money |
| TourPackage | 11 | 9 | City, DateRange, Excursion, Flight, Hotel, Money |
| TravelAdvisory | 4 | 6 | Country, Date |
| TravelAgency | 15 | 19 | Booking, BookingManager, Country, CurrencyConverter, Date, DateRange, InsurancePolicy, LoyaltyAccount, Money, NotificationService, PaymentProcessor, PriceCalculator, PromoCode, ReviewBoard, SupportTicket, TravelCatalog, Traveler, UserAccount |
| TravelAgent | 3 | 6 | Booking, Money, Staff |
| TravelAnalytics | 2 | 6 | Booking, Date |
| TravelCatalog | 5 | 14 | Attraction, City, Country, Date, DateRange, Flight, Hotel, TourPackage, TravelAdvisory |
| TravelPreferences | 5 | 5 | Country, Money |
| Traveler | 3 | 7 | ContactInfo, Country, Date, DateRange, Passport, Person, Visa |
| UserAccount | 6 | 7 | Traveler |
| Visa | 4 | 4 | Country, Date, DateRange |
| Exceptions | 2 | 0 | TravelException, InvalidDateException, InvalidDateRangeException и др. |

### Исключения (21)

- TravelException
- InvalidDateException (наследник TravelException)
- InvalidDateRangeException (наследник TravelException)
- InvalidDataException (наследник TravelException)
- InvalidDocumentException (наследник TravelException)
- VisaRequiredException (наследник TravelException)
- FlightNotFoundException (наследник TravelException)
- NoSeatsAvailableException (наследник TravelException)
- RoomUnavailableException (наследник TravelException)
- BookingNotFoundException (наследник TravelException)
- InvalidStateException (наследник TravelException)
- PaymentDeclinedException (наследник TravelException)
- InsufficientPointsException (наследник TravelException)
- PromoCodeInvalidException (наследник TravelException)
- AccountLockedException (наследник TravelException)
- AuthenticationFailedException (наследник TravelException)
- WeakPasswordException (наследник TravelException)
- TravelRestrictionException (наследник TravelException)
- InsuranceClaimException (наследник TravelException)
- DuplicateEntityException (наследник TravelException)
- AccessDeniedException (наследник TravelException)

### Итоговая статистика

| Показатель | Значение |
| ---------- | -------- |
| Классы предметной области | 56 |
| Классы исключений | 21 |
| Поля | 265 |
| Поведения | 364 |
| Ассоциации | 153 |
| Исключения | 21 |

Требования задания: ≥ 50 классов, ≥ 150 полей, ≥ 100 поведений, ≥ 30 ассоциаций, ≥ 12 исключений. Поля исключений (2 шт.) в общее число не входят. Поведения подсчитаны построчно по каждому классу, перегруженные методы с одним именем считаются один раз.

## Подробное описание классов

### Address

Почтовый адрес.

**Поля:** `street_` — `std::string`; `city_` — `std::string`; `postalCode_` — `std::string`; `country_` — `std::string`

**Методы:** `fullAddress`, `isComplete`, `isDomestic`, `isSameCity`

**Ассоциации:** —

### AgencyManager

Руководитель агентства: одобряет возвраты в рамках лимита, разблокирует аккаунты.

**Поля:** `approvalLimit_` — `Money`; `approvedRefunds_` — `int`

**Методы:** `approveDiscount`, `approveRefund`, `canApproveRefund`, `canRefund`, `role`, `unlockAccount`

**Ассоциации:** Money, Staff, UserAccount

### Aircraft

Воздушное судно: число мест по классам, норма багажа.

**Поля:** `model_` — `std::string`; `economySeats_` — `int`; `businessSeats_` — `int`; `maxLuggageKg_` — `int`

**Методы:** `isWideBody`, `luggageAllowanceFor`, `seatsIn`, `totalSeats`

**Ассоциации:** —

### Airline

Авиакомпания с рейтингом надёжности.

**Поля:** `code_` — `std::string`; `name_` — `std::string`; `country_` — `Country`; `rating_` — `double`

**Методы:** `isBasedIn`, `isReliable`, `rate`

**Ассоциации:** Country

### Airport

Аэропорт с кодом IATA (3 заглавные буквы).

**Поля:** `code_` — `std::string`; `name_` — `std::string`; `city_` — `City`; `terminals_` — `int`

**Методы:** `distanceTo`, `isInternationalTo`, `isSameCity`, `isValidCode`

**Ассоциации:** City

### Attraction

Достопримечательность с часами работы и ценой билета.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `city_` — `City`; `ticketPrice_` — `Money`; `openHour_` — `int`; `closeHour_` — `int`

**Методы:** `isInCity`, `isOpenAt`, `openHours`, `ticketsCost`

**Ассоциации:** City, Money

### Booking

Базовое бронирование: статусы, отмена со штрафом, полиморфные условия отмены.

**Поля:** `id_` — `std::string`; `travelerId_` — `std::string`; `createdAt_` — `Date`; `status_` — `BookingStatus`; `price_` — `Money`

**Методы:** `cancel`, `cancellationFee`, `complete`, `confirm`, `describe`, `destination`, `isActive`, `isExclusive`, `markPaid`, `period`, `refundAmount`, `requireStatus`, `setPrice`, `type`

**Ассоциации:** Date, Money

### BookingManager

Реестр бронирований: поиск, отмена, статистика, выручка.

**Поля:** `bookings_` — `std::map<std::string, std::shared_ptr<Booking>>`; `nextNumber_` — `int`; `cancelledFees_` — `Money`

**Методы:** `activeFor`, `add`, `all`, `cancel`, `countByStatus`, `find`, `generateId`, `revenue`, `size`

**Ассоциации:** Booking, Date, Money

### CarRental

Автомобиль в аренду.

**Поля:** `id_` — `std::string`; `model_` — `std::string`; `pickupCity_` — `City`; `dailyPrice_` — `Money`; `seats_` — `int`; `available_` — `bool`

**Методы:** `costFor`, `fitsPassengers`, `giveBack`, `isAvailableIn`, `reserve`

**Ассоциации:** City, DateRange, Money

### CarRentalBooking

Бронирование аренды авто (наследник Booking).

**Поля:** `carId_` — `std::string`; `carModel_` — `std::string`; `rental_` — `DateRange`; `pickupCity_` — `std::string`

**Методы:** `cancellationFee`, `describe`, `destination`, `lateReturnFee`, `period`, `type`

**Ассоциации:** Booking, Date, DateRange, Money

### City

Город со страной, координатами и часовым поясом.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `country_` — `Country`; `location_` — `GeoPoint`; `utcOffset_` — `int`

**Методы:** `distanceTo`, `isInCountry`, `isSameCountry`, `timeDifferenceHours`

**Ассоциации:** Country, GeoPoint

### ContactInfo

Телефон и e-mail с проверкой формата.

**Поля:** `phone_` — `std::string`; `email_` — `std::string`

**Методы:** `hasAnyContact`, `isEmailValid`, `isPhoneValid`, `maskedPhone`

**Ассоциации:** —

### Country

Страна: валюта и правила безвизового въезда.

**Поля:** `code_` — `std::string`; `name_` — `std::string`; `currency_` — `std::string`; `visaFreeFor_` — `std::vector<std::string>`

**Методы:** `addVisaFreeCountry`, `isSameCountry`, `isVisaFreeFor`, `usesCurrency`

**Ассоциации:** —

### CurrencyConverter

Конвертер валют через базовую валюту.

**Поля:** `baseCurrency_` — `std::string`; `rates_` — `std::map<std::string, double>`

**Методы:** `convert`, `rateOf`, `setRate`, `supports`

**Ассоциации:** Money

### Date

Календарная дата с арифметикой и днём недели.

**Поля:** `year_` — `int`; `month_` — `int`; `day_` — `int`

**Методы:** `addDays`, `dayOfWeek`, `daysInMonth`, `daysUntil`, `fromDayNumber`, `isBefore`, `isLeapYear`, `isValidDate`, `isWeekend`, `toDayNumber`, `toString`

**Ассоциации:** —

### DateRange

Период между двумя датами.

**Поля:** `start_` — `Date`; `end_` — `Date`

**Методы:** `contains`, `extend`, `nights`, `overlaps`, `startsWithin`, `toString`

**Ассоциации:** Date

### Excursion

Экскурсия: гид, достопримечательность, вместимость.

**Поля:** `id_` — `std::string`; `title_` — `std::string`; `attraction_` — `Attraction`; `guide_` — `Guide`; `date_` — `Date`; `capacity_` — `int`; `booked_` — `int`; `price_` — `Money`

**Методы:** `bookPlaces`, `cancelPlaces`, `freePlaces`, `isFull`, `isOnDate`, `totalCost`

**Ассоциации:** Attraction, Date, Guide, Money

### ExcursionBooking

Бронирование экскурсии (наследник Booking).

**Поля:** `excursionId_` — `std::string`; `title_` — `std::string`; `places_` — `int`; `date_` — `Date`

**Методы:** `cancellationFee`, `describe`, `period`, `type`

**Ассоциации:** Booking, Date, Money

### Flight

Рейс: самолёт, места, цена по классам, время прилёта.

**Поля:** `number_` — `std::string`; `airline_` — `Airline`; `origin_` — `Airport`; `destination_` — `Airport`; `departure_` — `Date`; `departureHour_` — `int`; `durationMinutes_` — `int`; `aircraft_` — `Aircraft`; `seats_` — `std::vector<Seat>`; `basePrice_` — `Money`

**Методы:** `arrivalDate`, `availableSeats`, `buildSeats`, `departsOn`, `distanceKm`, `findSeat`, `hasFreeSeat`, `isFull`, `isInternational`, `occupancyPercent`, `priceFor`, `releaseSeat`, `reserveSeat`, `reserveSpecificSeat`

**Ассоциации:** Aircraft, Airline, Airport, Date, Money, Seat

### FlightBooking

Бронирование авиабилета (наследник Booking).

**Поля:** `flightNumber_` — `std::string`; `seatLabel_` — `std::string`; `cabin_` — `CabinClass`; `departure_` — `Date`; `route_` — `std::string`; `passengerName_` — `std::string`

**Методы:** `baggageAllowanceKg`, `cancellationFee`, `changeSeat`, `describe`, `destination`, `isCheckInOpen`, `period`, `type`

**Ассоциации:** Booking, Date, Money

### GeoPoint

Географическая точка, расстояние по формуле гаверсинусов.

**Поля:** `latitude_` — `double`; `longitude_` — `double`

**Методы:** `distanceKm`, `isEasternHemisphere`, `isNear`, `isNorthernHemisphere`

**Ассоциации:** —

### Guide

Гид-экскурсовод (наследник Staff).

**Поля:** `languages_` — `std::vector<std::string>`; `hourlyRate_` — `Money`; `certified_` — `bool`

**Методы:** `addLanguage`, `canGuide`, `certify`, `costFor`, `role`, `speaks`

**Ассоциации:** Attraction, Money, Staff

### Hotel

Отель: номера, удобства, бронирование и отмена номера.

**Поля:** `id_` — `std::string`; `name_` — `std::string`; `city_` — `City`; `address_` — `Address`; `stars_` — `int`; `rooms_` — `std::vector<Room>`; `amenities_` — `std::vector<std::string>`; `rating_` — `double`

**Методы:** `addAmenity`, `addRoom`, `availableRoomsCount`, `averagePrice`, `bookRoom`, `cancelRoom`, `findRoom`, `hasAmenity`, `isLuxury`, `updateRating`

**Ассоциации:** Address, City, DateRange, Room

### HotelBooking

Бронирование проживания (наследник Booking).

**Поля:** `hotelId_` — `std::string`; `hotelName_` — `std::string`; `roomNumber_` — `int`; `stay_` — `DateRange`; `guests_` — `int`; `city_` — `std::string`

**Методы:** `cancellationFee`, `describe`, `destination`, `extendStay`, `isExclusive`, `nightsCount`, `period`, `type`

**Ассоциации:** Booking, Date, DateRange, Money

### InsuranceClaim

Заявка на страховой случай.

**Поля:** `id_` — `std::string`; `amount_` — `Money`; `reason_` — `std::string`; `filedAt_` — `Date`; `status_` — `ClaimStatus`

**Методы:** `approve`, `isPending`, `payout`, `reject`

**Ассоциации:** Date, Money

### InsurancePolicy

Страховой полис: тип, покрытие, подача и одобрение заявок.

**Поля:** `id_` — `std::string`; `bookingId_` — `std::string`; `type_` — `InsuranceType`; `coverage_` — `Money`; `premium_` — `Money`; `period_` — `DateRange`; `claims_` — `std::vector<InsuranceClaim>`

**Методы:** `approveClaim`, `covers`, `fileClaim`, `isActiveOn`, `premiumFor`, `remainingCoverage`, `totalPaidOut`

**Ассоциации:** Date, DateRange, InsuranceClaim, Money

### Itinerary

Маршрут путешествия из нескольких бронирований, поиск пересечений.

**Поля:** `id_` — `std::string`; `title_` — `std::string`; `travelerId_` — `std::string`; `bookings_` — `std::vector<std::shared_ptr<Booking>>`; `createdAt_` — `Date`

**Методы:** `addBooking`, `destinations`, `hasConflicts`, `isFullyPaid`, `overlappingPairs`, `removeBooking`, `totalPeriod`, `totalPrice`

**Ассоциации:** Booking, Date

### LoyaltyAccount

Бонусный счёт: баллы, уровни, скидки.

**Поля:** `travelerId_` — `std::string`; `points_` — `int`; `tier_` — `LoyaltyTier`; `joinedAt_` — `Date`

**Методы:** `discountPercent`, `earnFromPurchase`, `pointsValue`, `recalculateTier`, `redeem`

**Ассоциации:** Date, Money

### Money

Денежная сумма в копейках с валютой (без ошибок округления double).

**Поля:** `cents_` — `long long`; `currency_` — `std::string`

**Методы:** `add`, `divide`, `fromUnits`, `isGreaterThan`, `isNegative`, `isZero`, `multiply`, `percentOf`, `requireSameCurrency`, `subtract`, `toString`

**Ассоциации:** —

### Notification

Уведомление пользователю.

**Поля:** `id_` — `int`; `recipientId_` — `std::string`; `message_` — `std::string`; `sentAt_` — `Date`; `read_` — `bool`

**Методы:** `isOlderThan`, `isUnread`, `markRead`, `preview`

**Ассоциации:** Date

### NotificationService

Отправка и хранение уведомлений, очистка устаревших.

**Поля:** `notifications_` — `std::vector<Notification>`; `nextId_` — `int`; `retentionDays_` — `int`

**Методы:** `countFor`, `markAllRead`, `notifyBooking`, `purgeOld`, `send`, `unreadFor`

**Ассоциации:** Booking, Date, Notification

### Passport

Паспорт: формат номера, срок действия, проверка на поездку.

**Поля:** `number_` — `std::string`; `holderName_` — `std::string`; `issuingCountry_` — `Country`; `expiry_` — `Date`

**Методы:** `daysToExpiry`, `expiresSoon`, `isFormatCorrect`, `isValidForTrip`, `isValidOn`

**Ассоциации:** Country, Date, DateRange

### Payment

Платёж с жизненным циклом статусов и частичными возвратами.

**Поля:** `id_` — `int`; `bookingId_` — `std::string`; `amount_` — `Money`; `method_` — `PaymentMethod`; `status_` — `PaymentStatus`; `createdAt_` — `Date`; `refunded_` — `Money`

**Методы:** `authorize`, `capture`, `decline`, `fee`, `isSuccessful`, `refund`, `refundableAmount`

**Ассоциации:** Date, Money

### PaymentProcessor

Приём платежей, лимиты, комиссия, возвраты.

**Поля:** `payments_` — `std::vector<Payment>`; `feePercent_` — `double`; `nextId_` — `int`; `dailyLimit_` — `Money`

**Методы:** `charge`, `feeFor`, `findPayment`, `paymentsFor`, `refund`, `successfulCount`, `totalCollected`, `totalRefunded`

**Ассоциации:** Booking, Date, Money, Payment

### Person

Базовый класс человека.

**Поля:** `id_` — `std::string`; `firstName_` — `std::string`; `lastName_` — `std::string`; `contact_` — `ContactInfo`; `birthDate_` — `Date`

**Методы:** `ageOn`, `fullName`, `isAdult`, `updateContact`

**Ассоциации:** ContactInfo, Date

### PriceCalculator

Сезонные цены, наценка за выходные, групповые скидки.

**Поля:** `highSeasonMultiplier_` — `double`; `weekendSurchargePercent_` — `double`; `groupDiscountThreshold_` — `int`; `groupDiscountPercent_` — `double`

**Методы:** `groupDiscount`, `isHighSeason`, `seasonalPrice`, `totalForStay`, `weekendPrice`

**Ассоциации:** Date, DateRange, Money

### PromoCode

Промокод со сроком действия, минимальной суммой и лимитом.

**Поля:** `code_` — `std::string`; `percent_` — `double`; `validity_` — `DateRange`; `minAmount_` — `Money`; `usageLimit_` — `int`; `used_` — `int`

**Методы:** `applyTo`, `isApplicable`, `isValidOn`, `registerUse`, `remainingUses`

**Ассоциации:** Date, DateRange, Money

### RefundCalculator

Расчёт возврата с учётом штрафа, комиссии и окна возврата.

**Поля:** `processingFee_` — `Money`; `minRefund_` — `Money`; `refundWindowDays_` — `int`

**Методы:** `isRefundable`, `isWithinWindow`, `refundFor`, `refundSharePercent`

**Ассоциации:** Booking, Date, Money

### Review

Отзыв с оценкой 1–5.

**Поля:** `id_` — `std::string`; `authorId_` — `std::string`; `targetId_` — `std::string`; `rating_` — `int`; `text_` — `std::string`; `date_` — `Date`

**Методы:** `isNegative`, `isPositive`, `isRecent`, `wordCount`

**Ассоциации:** Date

### ReviewBoard

Доска отзывов: модерация, дубликаты, средние оценки.

**Поля:** `reviews_` — `std::vector<Review>`; `bannedWords_` — `std::vector<std::string>`

**Методы:** `averageFor`, `countFor`, `positiveShare`, `recentFor`, `submit`

**Ассоциации:** Date, Review

### Room

Номер отеля: даты занятости, цена за ночь.

**Поля:** `number_` — `int`; `type_` — `RoomType`; `capacity_` — `int`; `pricePerNight_` — `Money`; `bookedRanges_` — `std::vector<DateRange>`

**Методы:** `book`, `cancel`, `costFor`, `fits`, `isAvailable`, `nightsBooked`

**Ассоциации:** DateRange, Money

### Seat

Место в самолёте.

**Поля:** `row_` — `int`; `letter_` — `char`; `cabin_` — `CabinClass`; `taken_` — `bool`

**Методы:** `isAisle`, `isWindow`, `label`, `release`, `reserve`

**Ассоциации:** —

### Staff

Сотрудник агентства (наследник Person).

**Поля:** `department_` — `std::string`; `hireDate_` — `Date`; `salary_` — `Money`

**Методы:** `canRefund`, `raiseSalary`, `role`, `yearsOfService`

**Ассоциации:** ContactInfo, Date, Money, Person

### SupportOperator

Оператор поддержки с ограничением нагрузки (наследник Staff).

**Поля:** `openTickets_` — `int`; `resolvedTotal_` — `int`; `maxTickets_` — `int`

**Методы:** `isOverloaded`, `replyTo`, `resolveTicket`, `role`, `takeTicket`, `workloadPercent`

**Ассоциации:** Staff, SupportTicket

### SupportTicket

Обращение в поддержку с жизненным циклом статусов.

**Поля:** `id_` — `int`; `travelerId_` — `std::string`; `bookingId_` — `std::string`; `subject_` — `std::string`; `status_` — `TicketStatus`; `createdAt_` — `Date`; `messages_` — `std::vector<std::string>`; `assignedTo_` — `std::string`

**Методы:** `addMessage`, `ageDays`, `assignTo`, `close`, `isOpen`, `isStale`, `reopen`, `resolve`

**Ассоциации:** Date

### TourBooking

Бронирование тура (наследник Booking).

**Поля:** `tourId_` — `std::string`; `tourTitle_` — `std::string`; `travelers_` — `int`; `tripPeriod_` — `DateRange`; `destinationCity_` — `std::string`

**Методы:** `cancellationFee`, `describe`, `destination`, `isExclusive`, `perPerson`, `period`, `removeTravelers`, `type`

**Ассоциации:** Booking, Date, DateRange, Money

### TourPackage

Готовый тур: перелёты, отель, экскурсии, проверка согласованности.

**Поля:** `id_` — `std::string`; `title_` — `std::string`; `destination_` — `City`; `period_` — `DateRange`; `hotel_` — `Hotel`; `outbound_` — `Flight`; `inbound_` — `Flight`; `excursions_` — `std::vector<Excursion>`; `basePrice_` — `Money`; `maxTravelers_` — `int`; `booked_` — `int`

**Методы:** `addExcursion`, `durationNights`, `freePlaces`, `isInternational`, `pricePerPerson`, `releasePlaces`, `reservePlaces`, `totalPrice`, `validateItinerary`

**Ассоциации:** City, DateRange, Excursion, Flight, Hotel, Money

### TravelAdvisory

Предупреждение о безопасности поездки в страну.

**Поля:** `country_` — `Country`; `level_` — `int`; `message_` — `std::string`; `issuedAt_` — `Date`

**Методы:** `escalate`, `forbidsTravel`, `isExpired`, `isSafe`, `relax`, `requiresWarning`

**Ассоциации:** Country, Date

### TravelAgency

Фасад: аккаунты, бронирование, оплата, отмена, страховки, поддержка.

**Поля:** `name_` — `std::string`; `catalog_` — `TravelCatalog`; `accounts_` — `std::vector<UserAccount>`; `loyalty_` — `std::vector<LoyaltyAccount>`; `bookings_` — `BookingManager`; `payments_` — `PaymentProcessor`; `notifications_` — `NotificationService`; `reviews_` — `ReviewBoard`; `promoCodes_` — `std::vector<PromoCode>`; `converter_` — `CurrencyConverter`; `pricing_` — `PriceCalculator`; `tickets_` — `std::vector<SupportTicket>`; `insurance_` — `std::vector<InsurancePolicy>`; `nextTicketId_` — `int`; `nextPolicyId_` — `int`

**Методы:** `addPromo`, `addReview`, `bookFlight`, `bookHotel`, `bookTour`, `buyInsurance`, `cancelBooking`, `checkAdvisory`, `checkDocuments`, `convertPrice`, `findAccount`, `findPromo`, `findTicket`, `login`, `loyaltyFor`, `openTicket`, `pay`, `registerAccount`, `releaseResources`

**Ассоциации:** Booking, BookingManager, Country, CurrencyConverter, Date, DateRange, InsurancePolicy, LoyaltyAccount, Money, NotificationService, PaymentProcessor, PriceCalculator, PromoCode, ReviewBoard, SupportTicket, TravelCatalog, Traveler, UserAccount

### TravelAgent

Турагент с комиссией (наследник Staff).

**Поля:** `commissionPercent_` — `double`; `salesCount_` — `int`; `languages_` — `std::vector<std::string>`

**Методы:** `addLanguage`, `commissionFor`, `monthlyBonus`, `registerSale`, `role`, `speaks`

**Ассоциации:** Booking, Money, Staff

### TravelAnalytics

Аналитика по бронированиям: выручка, отмены, популярные направления.

**Поля:** `title_` — `std::string`; `generatedAt_` — `Date`

**Методы:** `averageBookingValue`, `cancellationRate`, `countByType`, `mostPopularDestination`, `revenue`, `summary`

**Ассоциации:** Booking, Date

### TravelCatalog

Каталог рейсов, отелей, туров, достопримечательностей и предупреждений.

**Поля:** `flights_` — `std::vector<Flight>`; `hotels_` — `std::vector<Hotel>`; `tours_` — `std::vector<TourPackage>`; `attractions_` — `std::vector<Attraction>`; `advisories_` — `std::vector<TravelAdvisory>`

**Методы:** `addAdvisory`, `addAttraction`, `addFlight`, `addHotel`, `addTour`, `advisoryFor`, `attractionsIn`, `cheapestFlightNumber`, `findFlight`, `findHotel`, `findHotels`, `findTour`, `searchFlights`, `toursTo`

**Ассоциации:** Attraction, City, Country, Date, DateRange, Flight, Hotel, TourPackage, TravelAdvisory

### TravelPreferences

Предпочтения путешественника: бюджет, класс, пересадки.

**Поля:** `budgetLimit_` — `Money`; `cabin_` — `CabinClass`; `maxStops_` — `int`; `favoriteCountries_` — `std::vector<std::string>`; `directFlightsOnly_` — `bool`

**Методы:** `acceptsStops`, `addFavorite`, `fitsBudget`, `isFavorite`, `wantsComfort`

**Ассоциации:** Country, Money

### Traveler

Путешественник: паспорт, гражданство, визы (наследник Person).

**Поля:** `passport_` — `Passport`; `citizenship_` — `Country`; `visas_` — `std::vector<Visa>`

**Методы:** `addVisa`, `canEnter`, `hasValidPassport`, `hasVisaFor`, `isMinor`, `requireDocuments`, `updatePassport`

**Ассоциации:** ContactInfo, Country, Date, DateRange, Passport, Person, Visa

### UserAccount

Учётная запись: проверка пароля, блокировка после 3 ошибок.

**Поля:** `login_` — `std::string`; `passwordHash_` — `std::string`; `traveler_` — `Traveler`; `active_` — `bool`; `failedAttempts_` — `int`; `locked_` — `bool`

**Методы:** `authenticate`, `changePassword`, `deactivate`, `hashPassword`, `isLocked`, `isStrongPassword`, `unlock`

**Ассоциации:** Traveler

### Visa

Виза: страна, срок, число въездов.

**Поля:** `country_` — `Country`; `validity_` — `DateRange`; `type_` — `std::string`; `entries_` — `int`

**Методы:** `coversTrip`, `entriesLeft`, `isActiveOn`, `useEntry`

**Ассоциации:** Country, Date, DateRange
