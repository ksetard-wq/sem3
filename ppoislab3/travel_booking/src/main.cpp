// Консольное приложение «Система бронирования путешествий».
// Все классы предметной области находятся в include/*.h, здесь — только консоль и сценарии.
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

#include "AgencyManager.h"
#include "CarRental.h"
#include "CarRentalBooking.h"
#include "ExcursionBooking.h"
#include "Guide.h"
#include "Itinerary.h"
#include "RefundCalculator.h"
#include "SupportOperator.h"
#include "TravelAgent.h"
#include "TravelAgency.h"
#include "TravelAnalytics.h"

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

Staff makeStaff(const std::string& id, const std::string& first, const std::string& last, int birthYear,
                const std::string& department, double salary, const Date& hired) {
    return Staff(id, first, last, ContactInfo("+7 (900) 111-22-" + id.substr(1), "staff" + id + "@travel.example"),
                 Date(birthYear, 4, 20), department, Money::fromUnits(salary), hired);
}

class ConsoleApp {
    Date today_;
    TravelAgency agency_;
    AgencyManager manager_;
    SupportOperator operator_;
    TravelAgent agent_;
    RefundCalculator refunds_;
    std::vector<City> cities_;
    std::vector<CarRental> cars_;
    std::vector<Excursion> excursions_;
    std::string currentLogin_;

public:
    ConsoleApp()
        : today_(todayDate()),
          agency_("TravelGo", makeConverter(), PriceCalculator()),
          manager_(makeStaff("S01", "Анна", "Захарова", 1980, "Руководство", 150000, todayDate().addDays(-2000)), Money::fromUnits(100000)),
          operator_(makeStaff("S02", "Павел", "Лебедев", 1992, "Поддержка", 70000, todayDate().addDays(-800)), 3),
          agent_(makeStaff("S03", "Ирина", "Белова", 1989, "Продажи", 80000, todayDate().addDays(-1200)), 5.0, {"русский", "английский"}) {
        seedData();
    }

    static CurrencyConverter makeConverter() {
        CurrencyConverter c("RUB");
        c.setRate("USD", 90.0);
        c.setRate("EUR", 100.0);
        c.setRate("TRY", 3.0);
        return c;
    }

    // ------------------------------------------------------------------ данные

    void seedData() {
        Country ru("RU", "Россия", "RUB"), tr("TR", "Турция", "TRY"), fr("FR", "Франция", "EUR");
        Country th("TH", "Таиланд", "THB"), jp("JP", "Япония", "JPY"), is("IS", "Исландия", "ISK");
        tr.addVisaFreeCountry(ru);
        th.addVisaFreeCountry(ru);

        City moscow("MOW", "Москва", ru, GeoPoint(55.75, 37.61), 3), istanbul("IST", "Стамбул", tr, GeoPoint(41.01, 28.97), 3);
        City antalya("AYT", "Анталья", tr, GeoPoint(36.90, 30.71), 3), paris("PAR", "Париж", fr, GeoPoint(48.86, 2.35), 1);
        City bangkok("BKK", "Бангкок", th, GeoPoint(13.75, 100.50), 7), tokyo("TYO", "Токио", jp, GeoPoint(35.68, 139.69), 9);
        City reykjavik("REK", "Рейкьявик", is, GeoPoint(64.15, -21.94), 0);
        cities_ = {moscow, istanbul, antalya, paris, bangkok, tokyo, reykjavik};

        Airport svo("SVO", "Шереметьево", moscow, 6), ist("IST", "Стамбул", istanbul, 2), ayt("AYT", "Анталья", antalya, 2);
        Airport cdg("CDG", "Шарль-де-Голль", paris, 3), bkk("BKK", "Суварнабхуми", bangkok, 1), nrt("NRT", "Нарита", tokyo, 2);
        Airport kef("KEF", "Кефлавик", reykjavik, 1);

        Airline su("SU", "Аэрофлот", ru, 4.4), tk("TK", "Turkish Airlines", tr, 4.5), af("AF", "Air France", fr, 4.1);
        Aircraft a320("Airbus A320", 150, 12, 20), b777("Boeing 777", 300, 28, 40);

        TravelCatalog& catalog = agency_.getCatalog();
        auto d = [this](int days) { return today_.addDays(days); };
        catalog.addFlight(Flight("SU1000", su, svo, ayt, d(20), 8, 200, a320, Money::fromUnits(15000)));
        catalog.addFlight(Flight("TK412", tk, svo, ist, d(20), 10, 190, a320, Money::fromUnits(12000)));
        catalog.addFlight(Flight("TK413", tk, svo, ist, d(20), 18, 190, a320, Money::fromUnits(11000)));
        catalog.addFlight(Flight("SU2100", su, svo, cdg, d(25), 9, 240, a320, Money::fromUnits(22000)));
        catalog.addFlight(Flight("SU270", su, svo, bkk, d(40), 20, 540, b777, Money::fromUnits(45000)));
        catalog.addFlight(Flight("SU5", su, svo, nrt, d(30), 11, 560, b777, Money::fromUnits(60000)));
        catalog.addFlight(Flight("SU9", su, svo, kef, d(35), 7, 300, a320, Money::fromUnits(30000)));

        auto makeHotel = [&](const std::string& id, const std::string& name, const City& city, int stars, double base) {
            Hotel h(id, name, city, Address("Центральная ул., 1", city.getName(), "000000", city.getCountry().getName()), stars);
            h.addRoom(Room(101, RoomType::Double, 2, Money::fromUnits(base)));
            h.addRoom(Room(102, RoomType::Double, 2, Money::fromUnits(base)));
            h.addRoom(Room(201, RoomType::Suite, 3, Money::fromUnits(base * 2.5)));
            h.addRoom(Room(301, RoomType::Family, 4, Money::fromUnits(base * 1.5)));
            h.addAmenity("Wi-Fi");
            h.addAmenity("Завтрак");
            return h;
        };
        Hotel seaBreeze = makeHotel("H1", "Sea Breeze Resort", antalya, 5, 6000);
        Hotel petit = makeHotel("H2", "Le Petit Paris", paris, 3, 8000);
        Hotel bosphorus = makeHotel("H3", "Bosphorus Inn", istanbul, 4, 5000);
        seaBreeze.addAmenity("Бассейн");
        catalog.addHotel(seaBreeze);
        catalog.addHotel(petit);
        catalog.addHotel(bosphorus);

        Guide guide(makeStaff("S04", "Мехмет", "Йылмаз", 1985, "Экскурсии", 50000, d(-900)), {"русский", "английский", "турецкий"}, Money::fromUnits(1500));
        guide.certify();
        Attraction kaleici("AT1", "Старый город Калеичи", antalya, Money::fromUnits(500), 9, 20);
        Attraction louvre("AT2", "Лувр", paris, Money::fromUnits(1800), 9, 18);
        catalog.addAttraction(kaleici);
        catalog.addAttraction(louvre);
        excursions_.emplace_back("E1", "Прогулка по Калеичи", kaleici, guide, d(22), 15, Money::fromUnits(2500));

        TourPackage tourAyt("T1", "Неделя в Анталье", antalya, DateRange(d(20), d(27)), seaBreeze,
                            Flight("SU1000", su, svo, ayt, d(20), 8, 200, a320, Money::fromUnits(15000)),
                            Flight("SU1001", su, ayt, svo, d(27), 9, 200, a320, Money::fromUnits(15000)), Money::fromUnits(60000), 10);
        tourAyt.addExcursion(excursions_.front());
        catalog.addTour(tourAyt);
        catalog.addTour(TourPackage("T2", "Париж за 5 дней", paris, DateRange(d(25), d(30)), petit,
                                    Flight("SU2100", su, svo, cdg, d(25), 9, 240, a320, Money::fromUnits(22000)),
                                    Flight("AF1145", af, cdg, svo, d(30), 14, 230, a320, Money::fromUnits(21000)), Money::fromUnits(85000), 6));

        catalog.addAdvisory(TravelAdvisory(fr, 2, "Повышенная бдительность в местах скопления людей", today_));
        catalog.addAdvisory(TravelAdvisory(is, 4, "Извержение вулкана, аэропорты могут быть закрыты", today_));

        cars_.emplace_back("CAR1", "Toyota Corolla", antalya, Money::fromUnits(3500), 5);
        cars_.emplace_back("CAR2", "Renault Clio", paris, Money::fromUnits(4200), 4);

        agency_.addPromo(PromoCode("WELCOME10", 10.0, DateRange(d(-5), d(60)), Money::fromUnits(5000), 100));
        agency_.addPromo(PromoCode("EXPIRED", 15.0, DateRange(d(-400), d(-300)), Money(), 10));

        addUser("ivan", "Ivan2026pass", "Иван", "Петров", 1990, "751234567", d(3000), ru, "ivan@example.com", &fr, &moscow);
        addUser("maria", "Maria2026pass", "Мария", "Сидорова", 1985, "752345678", d(200), ru, "maria@example.com", nullptr, nullptr);
        addUser("oleg", "Oleg2026pass", "Олег", "Иванов", 1995, "753456789", d(3000), ru, "oleg@example.com", nullptr, nullptr);
    }

    void addUser(const std::string& login, const std::string& password, const std::string& first, const std::string& last,
                 int birthYear, const std::string& passportNumber, const Date& passportExpiry, const Country& citizenship,
                 const std::string& email, const Country* visaCountry, const City*) {
        Passport passport(passportNumber, last + " " + first, citizenship, passportExpiry);
        Traveler traveler("T-" + login, first, last, ContactInfo("+7 (900) 123-45-67", email), Date(birthYear, 6, 15), passport, citizenship);
        if (visaCountry != nullptr) traveler.addVisa(Visa(*visaCountry, DateRange(today_.addDays(-10), today_.addDays(365)), "Шенген", 2));
        agency_.registerAccount(UserAccount(login, password, traveler), today_);
    }

    // ------------------------------------------------------------ сценарии

    void requireLogin() const {
        if (currentLogin_.empty()) throw AuthenticationFailedException("вход не выполнен");
    }

    Traveler& currentTraveler() {
        requireLogin();
        return agency_.findAccount(currentLogin_).getTraveler();
    }

    void printBooking(const Booking& b) const {
        std::cout << "  " << b.getId() << " [" << toString(b.getStatus()) << "] " << b.describe() << " — " << b.getPrice().toString() << "\n";
    }

    void showFlights() {
        for (const auto& f : agency_.getCatalog().getFlights()) {
            std::cout << f.getNumber() << " " << f.getOrigin().getCode() << "→" << f.getDestination().getCode() << " "
                      << f.getDeparture().toString() << " " << f.getDepartureHour() << ":00, " << f.getAirline().getName()
                      << ", " << static_cast<int>(f.distanceKm()) << " км, эконом " << f.priceFor(CabinClass::Economy).toString()
                      << " (свободно " << f.availableSeats(CabinClass::Economy) << "), бизнес " << f.priceFor(CabinClass::Business).toString()
                      << " (свободно " << f.availableSeats(CabinClass::Business) << ")" << (f.isInternational() ? ", междунар." : "") << "\n";
        }
    }

    int chooseCity(const std::string& prompt) {
        for (size_t i = 0; i < cities_.size(); ++i) std::cout << "  " << i + 1 << ". " << cities_[i].getName() << "\n";
        return readInt(prompt + ": ", 1, static_cast<int>(cities_.size())) - 1;
    }

    void searchFlights() {
        int from = chooseCity("Откуда"), to = chooseCity("Куда");
        Date date = today_.addDays(readInt("Через сколько дней вылет (0-365): ", 0, 365));
        auto found = agency_.getCatalog().searchFlights(cities_[from], cities_[to], date);
        if (found.empty()) std::cout << "Рейсов не найдено.\n";
        for (const auto& f : found) {
            std::cout << f.getNumber() << " " << f.getDepartureHour() << ":00, прилёт " << f.arrivalDate().toString() << ", " << f.getAirline().getName()
                      << ", эконом " << f.priceFor(CabinClass::Economy).toString() << "\n";
        }
        if (!found.empty()) {
            std::cout << "Самый дешёвый: " << agency_.getCatalog().cheapestFlightNumber(cities_[from], cities_[to], date, CabinClass::Economy) << "\n";
        }
    }

    void searchHotels() {
        int city = chooseCity("Город");
        int offset = readInt("Заезд через (дней, 0-365): ", 0, 365);
        int nights = readInt("Ночей (1-30): ", 1, 30);
        DateRange stay(today_.addDays(offset), today_.addDays(offset + nights));
        int guests = readInt("Гостей (1-6): ", 1, 6);
        auto hotels = agency_.getCatalog().findHotels(cities_[city], stay, guests);
        if (hotels.empty()) std::cout << "Свободных отелей нет.\n";
        for (const auto& h : hotels) {
            std::cout << h.getId() << " " << h.getName() << " " << h.getStars() << "★, средняя цена "
                      << h.averagePrice().toString() << ", свободно номеров: " << h.availableRoomsCount(stay)
                      << ", отзывов: " << agency_.getReviews().countFor(h.getId()) << ", оценка гостей "
                      << agency_.getReviews().averageFor(h.getId()) << (h.hasAmenity("бассейн") ? ", есть бассейн" : "") << "\n";
        }
    }

    void showTours() {
        for (const auto& t : agency_.getCatalog().getTours()) {
            std::cout << t.getId() << " «" << t.getTitle() << "» " << t.getPeriod().toString() << " (" << t.durationNights() << " ноч.), "
                      << t.pricePerPerson().toString() << "/чел., мест: " << t.freePlaces() << ", экскурсий: " << t.getExcursions().size()
                      << (t.isInternational() ? ", междунар." : "") << "\n";
        }
    }

    void loginMenu() {
        std::string login = readLine("Логин (ivan / maria / oleg): ");
        std::string password = readLine("Пароль: ");
        UserAccount& account = agency_.login(login, password);
        currentLogin_ = login;
        std::cout << "Вход выполнен: " << account.getTraveler().fullName() << ", бонусных баллов: "
                  << agency_.loyaltyFor(account.getTraveler().getId()).getPoints() << "\n";
    }

    void bookFlightMenu() {
        requireLogin();
        showFlights();
        std::string number = readLine("Номер рейса: ");
        int cabin = readInt("Класс: 1 — эконом, 2 — бизнес: ", 1, 2);
        std::string id = agency_.bookFlight(currentLogin_, number, cabin == 1 ? CabinClass::Economy : CabinClass::Business, today_);
        printBooking(*agency_.getBookings().find(id));
    }

    void bookHotelMenu() {
        requireLogin();
        for (const auto& h : agency_.getCatalog().getHotels()) std::cout << "  " << h.getId() << " " << h.getName() << " (" << h.getCity().getName() << ")\n";
        std::string hotelId = readLine("Код отеля: ");
        int type = readInt("Номер: 1 — одноместный, 2 — двухместный, 3 — люкс, 4 — семейный: ", 1, 4);
        int offset = readInt("Заезд через (дней, 0-365): ", 0, 365);
        int nights = readInt("Ночей (1-30): ", 1, 30);
        int guests = readInt("Гостей (1-6): ", 1, 6);
        std::string id = agency_.bookHotel(currentLogin_, hotelId, static_cast<RoomType>(type - 1),
                                           DateRange(today_.addDays(offset), today_.addDays(offset + nights)), guests, today_);
        printBooking(*agency_.getBookings().find(id));
    }

    void bookTourMenu() {
        requireLogin();
        showTours();
        std::string tourId = readLine("Код тура: ");
        int travelers = readInt("Путешественников (1-10): ", 1, 10);
        printBooking(*agency_.getBookings().find(agency_.bookTour(currentLogin_, tourId, travelers, today_)));
    }

    void carMenu() {
        requireLogin();
        for (size_t i = 0; i < cars_.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << cars_[i].getModel() << " (" << cars_[i].getPickupCity().getName() << "), "
                      << cars_[i].getDailyPrice().toString() << "/сутки" << (cars_[i].getAvailable() ? "" : " [занят]") << "\n";
        }
        CarRental& car = cars_[static_cast<size_t>(readInt("Автомобиль: ", 1, static_cast<int>(cars_.size()))) - 1];
        int offset = readInt("Начало аренды через (дней): ", 0, 365), days = readInt("Суток (1-30): ", 1, 30);
        DateRange rental(today_.addDays(offset), today_.addDays(offset + days));
        int passengers = readInt("Пассажиров: ", 1, 8);
        if (!car.fitsPassengers(passengers)) throw InvalidDataException("автомобиль не вмещает " + std::to_string(passengers) + " пассажиров");
        car.reserve();
        auto booking = std::make_shared<CarRentalBooking>(agency_.getBookings().generateId("CR"), currentTraveler().getId(), today_,
                                                          car.costFor(rental), car.getId(), car.getModel(), rental, car.getPickupCity().getName());
        booking->confirm();
        agency_.getBookings().add(booking);
        printBooking(*booking);
    }

    void excursionMenu() {
        requireLogin();
        for (const auto& e : excursions_) {
            std::cout << "  " << e.getId() << " " << e.getTitle() << ", " << e.getDate().toString() << ", мест " << e.freePlaces()
                      << ", гид " << e.getGuide().fullName() << ", цена " << e.getPrice().toString() << "\n";
        }
        std::string id = readLine("Код экскурсии: ");
        int places = readInt("Мест (1-20): ", 1, 20);
        for (auto& e : excursions_) {
            if (e.getId() != id) continue;
            e.bookPlaces(places);
            auto booking = std::make_shared<ExcursionBooking>(agency_.getBookings().generateId("EX"), currentTraveler().getId(), today_,
                                                              e.totalCost(places), e.getId(), e.getTitle(), places, e.getDate());
            booking->confirm();
            agency_.getBookings().add(booking);
            printBooking(*booking);
            return;
        }
        throw InvalidDataException("экскурсия не найдена: " + id);
    }

    void myBookings() {
        requireLogin();
        Traveler& traveler = currentTraveler();
        Itinerary trip("IT-" + traveler.getId(), "Мой маршрут", traveler.getId(), today_);
        for (const auto& b : agency_.getBookings().activeFor(traveler.getId())) {
            trip.addBooking(b);
            printBooking(*b);
        }
        if (trip.getBookings().empty()) {
            std::cout << "Активных броней нет.\n";
            return;
        }
        std::cout << "Итого: " << trip.totalPrice().toString() << ", период: " << trip.totalPeriod().toString()
                  << ", направления: ";
        for (const auto& d : trip.destinations()) std::cout << d << " ";
        std::cout << "\nПересечений проживания: " << trip.overlappingPairs() << ", всё оплачено: " << (trip.isFullyPaid() ? "да" : "нет") << "\n";
    }

    void payMenu() {
        requireLogin();
        myBookings();
        std::string id = readLine("Код брони для оплаты: ");
        int method = readInt("Способ: 1 — карта, 2 — наличные, 3 — перевод, 4 — кошелёк: ", 1, 4);
        std::string promo = readLine("Промокод (Enter — без скидки; WELCOME10): ");
        Payment payment = agency_.pay(id, static_cast<PaymentMethod>(method - 1), promo, today_);
        std::shared_ptr<Booking> booking = agency_.getBookings().find(id);
        std::cout << "Оплачено " << payment.getAmount().toString() << " (" << toString(payment.getMethod()) << "), комиссия банка "
                  << agency_.getPayments().feeFor(payment.getAmount()).toString() << ", бонусов: "
                  << agency_.loyaltyFor(booking->getTravelerId()).getPoints() << ", комиссия агента "
                  << agent_.registerSale(*booking).toString() << "\n";
    }

    void cancelMenu() {
        requireLogin();
        myBookings();
        std::string id = readLine("Код брони для отмены: ");
        std::shared_ptr<Booking> booking = agency_.getBookings().find(id);
        std::cout << "Штраф при отмене сегодня: " << booking->cancellationFee(today_).toString() << ", расчёт возврата: "
                  << refunds_.refundFor(*booking, today_).toString() << "\n";
        if (readInt("Отменить? 1 — да, 0 — нет: ", 0, 1) == 1) {
            Money refund = agency_.cancelBooking(id, today_);
            if (!refund.isZero()) manager_.approveRefund(refund);
            std::cout << "Бронь отменена, возврат клиенту: " << refund.toString() << "\n";
        }
    }

    void insuranceMenu() {
        requireLogin();
        myBookings();
        std::string id = readLine("Код брони для страховки: ");
        int type = readInt("Тип: 1 — базовая, 2 — расширенная, 3 — премиум: ", 1, 3);
        InsurancePolicy& policy = agency_.buyInsurance(id, static_cast<InsuranceType>(type - 1), today_);
        std::cout << "Полис " << policy.getId() << " (" << toString(policy.getType()) << "), покрытие " << policy.getCoverage().toString()
                  << ", взнос " << policy.getPremium().toString() << "\n";
        if (readInt("Подать заявку на страховой случай? 1 — да, 0 — нет: ", 0, 1) == 1) {
            std::string reason = readLine("Причина (болезнь, отмена, кража): ");
            policy.fileClaim(InsuranceClaim("CL-1", policy.getPremium().multiply(5), reason, today_), today_);
            policy.approveClaim("CL-1");
            std::cout << "Заявка одобрена. Остаток покрытия: " << policy.remainingCoverage().toString() << "\n";
        }
    }

    void supportMenu() {
        requireLogin();
        std::cout << "1. Создать обращение\n2. Обработать обращения (оператор)\n0. Назад\n";
        int c = readInt("> ", 0, 2);
        if (c == 1) {
            std::string booking = readLine("Код брони (Enter — без брони): ");
            int id = agency_.openTicket(currentLogin_, booking, readLine("Тема: "), today_);
            std::cout << "Создано обращение №" << id << "\n";
        } else if (c == 2) {
            for (const auto& t : agency_.getTickets()) {
                std::cout << "  №" << t.getId() << " [" << toString(t.getStatus()) << "] " << t.getSubject() << ", возраст " << t.ageDays(today_) << " дн.\n";
            }
            if (agency_.getTickets().empty()) return;
            SupportTicket& ticket = agency_.findTicket(readInt("Номер обращения: ", 1, 100000));
            if (ticket.getStatus() == TicketStatus::Open) operator_.takeTicket(ticket);
            operator_.replyTo(ticket, readLine("Ответ оператора: "));
            operator_.resolveTicket(ticket);
            std::cout << "Обращение решено. Загрузка оператора: " << operator_.workloadPercent() << "%\n";
        }
    }

    void reviewsMenu() {
        requireLogin();
        std::string target = readLine("Объект отзыва (код отеля, например H1): ");
        int rating = readInt("Оценка 1-5: ", 1, 5);
        agency_.addReview(currentLogin_, target, rating, readLine("Текст: "), today_);
        std::cout << "Средняя оценка " << target << ": " << agency_.getReviews().averageFor(target) << ", положительных: "
                  << agency_.getReviews().positiveShare(target) << "%\n";
    }

    void notificationsMenu() {
        requireLogin();
        std::string id = currentTraveler().getId();
        auto unread = agency_.getNotifications().unreadFor(id);
        for (const auto& n : unread) std::cout << "  [" << n.getSentAt().toString() << "] " << n.preview(70) << "\n";
        std::cout << "Непрочитанных: " << unread.size() << "\n";
        agency_.getNotifications().markAllRead(id);
    }

    void currencyMenu() {
        int amount = readInt("Сумма в рублях (1-10000000): ", 1, 10000000);
        Money rub = Money::fromUnits(amount);
        for (const char* cur : {"USD", "EUR", "TRY"}) std::cout << "  " << rub.toString() << " = " << agency_.convertPrice(rub, cur).toString() << "\n";
    }

    void analyticsMenu() {
        TravelAnalytics report("Отчёт агентства " + agency_.getName(), today_);
        auto all = agency_.getBookings().all();
        std::cout << report.summary(all);
        std::cout << "Собрано платежей: " << agency_.getPayments().totalCollected().toString() << ", возвращено: "
                  << agency_.getPayments().totalRefunded().toString() << ", штрафы за отмены: " << agency_.getBookings().getCancelledFees().toString() << "\n";
        for (const auto& b : all) {
            if (b->isActive()) std::cout << "  " << b->getId() << ": при отмене сегодня вернётся " << refunds_.refundSharePercent(*b, today_) << "%\n";
        }
    }

    // ---------------------------------------------------- демонстрации

    static void attempt(const std::string& title, const std::function<void()>& action) {
        try {
            action();
            std::cout << "[выполнено]  " << title << "\n";
        } catch (const TravelException& e) {
            std::cout << "[исключение] " << title << " -> " << e.what() << "\n";
        }
    }

    void exceptionsDemo() {
        Date today = today_;
        attempt("InvalidDateException: 30 февраля", [] { Date bad(2026, 2, 30); });
        attempt("InvalidDateRangeException: конец раньше начала", [&] { DateRange bad(today.addDays(5), today); });
        attempt("InvalidDataException: рубли + доллары", [] { Money(100).add(Money(100, "USD")); });
        attempt("InvalidDocumentException: паспорт Марии скоро истечёт", [&] { agency_.bookFlight("maria", "SU270", CabinClass::Economy, today); });
        attempt("VisaRequiredException: Олегу нужна виза во Францию", [&] { agency_.bookFlight("oleg", "SU2100", CabinClass::Economy, today); });
        attempt("FlightNotFoundException: рейс XX000", [&] { agency_.getCatalog().findFlight("XX000"); });
        attempt("NoSeatsAvailableException: 999 мест на экскурсию", [&] { excursions_.front().bookPlaces(999); });
        attempt("RoomUnavailableException: номер занят дважды", [&] {
            Room r(1, RoomType::Single, 1, Money::fromUnits(1000));
            DateRange period(today, today.addDays(2));
            r.book(period);
            r.book(period);
        });
        attempt("BookingNotFoundException: бронь ZZ-1", [&] { agency_.getBookings().find("ZZ-1"); });
        attempt("InvalidStateException: оплата неподтверждённой брони", [&] {
            Booking pending("X-1", "T", today, Money::fromUnits(100));
            pending.markPaid();
        });
        attempt("PaymentDeclinedException: платёж на 0", [&] { Payment p(1, "X", Money(), PaymentMethod::Card, today); p.authorize(); });
        attempt("InsufficientPointsException: списать 100000 баллов", [&] { agency_.loyaltyFor("T-ivan").redeem(100000); });
        attempt("PromoCodeInvalidException: просроченный промокод", [&] { agency_.findPromo("EXPIRED").applyTo(Money::fromUnits(10000), today); });
        attempt("AuthenticationFailedException: неверный пароль", [&] { agency_.login("oleg", "wrong"); });
        attempt("AuthenticationFailedException: вторая ошибка", [&] { agency_.login("oleg", "wrong"); });
        attempt("AccountLockedException: третья ошибка блокирует аккаунт", [&] { agency_.login("oleg", "wrong"); });
        manager_.unlockAccount(agency_.findAccount("oleg"));
        attempt("WeakPasswordException: пароль \"123\"", [&] {
            UserAccount bad("newuser", "123", agency_.findAccount("ivan").getTraveler());
        });
        attempt("TravelRestrictionException: Исландия (уровень 4)", [&] { agency_.bookFlight("ivan", "SU9", CabinClass::Economy, today); });
        attempt("InsuranceClaimException: кража не покрывается базовым полисом", [&] {
            InsurancePolicy policy("P", "B", InsuranceType::Basic, Money::fromUnits(1000), Money::fromUnits(30), DateRange(today, today.addDays(5)));
            policy.fileClaim(InsuranceClaim("C", Money::fromUnits(100), "кража", today), today);
        });
        attempt("DuplicateEntityException: повторная регистрация ivan", [&] { addUser("ivan", "Ivan2026pass", "И", "П", 1990, "751234567", today.addDays(3000),
                                                                                  agency_.findAccount("ivan").getTraveler().getCitizenship(), "x@y.ru", nullptr, nullptr); });
        attempt("AccessDeniedException: возврат выше лимита руководителя", [&] { manager_.approveRefund(Money::fromUnits(500000)); });
        attempt("TravelException (базовое): ручной вызов", [] { throw TravelException("общая ошибка системы"); });
    }

    void fullDemo() {
        std::cout << "== Сценарий: вход -> перелёт -> отель -> страховка -> оплата -> отмена ==\n";
        currentLogin_ = "ivan";
        agency_.login("ivan", "Ivan2026pass");
        std::string flight = agency_.bookFlight("ivan", "SU2100", CabinClass::Economy, today_);
        std::string hotel = agency_.bookHotel("ivan", "H2", RoomType::Double, DateRange(today_.addDays(25), today_.addDays(30)), 2, today_);
        std::string car = agency_.getBookings().generateId("CR");
        cars_[1].reserve();
        auto rental = std::make_shared<CarRentalBooking>(car, "T-ivan", today_, cars_[1].costFor(DateRange(today_.addDays(25), today_.addDays(30))),
                                                         cars_[1].getId(), cars_[1].getModel(), DateRange(today_.addDays(25), today_.addDays(30)), "Париж");
        rental->confirm();
        agency_.getBookings().add(rental);
        myBookings();

        InsurancePolicy& policy = agency_.buyInsurance(flight, InsuranceType::Extended, today_);
        std::cout << "Страховка " << policy.getId() << ": взнос " << policy.getPremium().toString() << "\n";
        Payment p1 = agency_.pay(flight, PaymentMethod::Card, "WELCOME10", today_);
        std::cout << "Оплата перелёта: " << p1.getAmount().toString() << ", комиссия агента " << agent_.registerSale(*agency_.getBookings().find(flight)).toString() << "\n";
        agency_.pay(hotel, PaymentMethod::Transfer, "", today_);
        Money refund = agency_.cancelBooking(flight, today_);
        std::cout << "Отмена перелёта: возврат " << refund.toString() << "\n";

        std::cout << "\n== Поддержка ==\n";
        int ticketId = agency_.openTicket("ivan", hotel, "Нужен поздний заезд", today_);
        SupportTicket& ticket = agency_.findTicket(ticketId);
        operator_.takeTicket(ticket);
        operator_.replyTo(ticket, "Согласовали заезд после 22:00");
        operator_.resolveTicket(ticket);
        ticket.close();
        std::cout << "Обращение №" << ticketId << ": " << toString(ticket.getStatus()) << "\n";

        std::cout << "\n== Исключения ==\n";
        exceptionsDemo();
        std::cout << "\n";
        analyticsMenu();
    }

    void run() {
        const char* menu =
            "\n=========== БРОНИРОВАНИЕ ПУТЕШЕСТВИЙ ===========\n"
            " 1. Все рейсы             2. Поиск рейсов          3. Поиск отелей\n"
            " 4. Туры                  5. Войти в аккаунт\n"
            " 6. Забронировать перелёт 7. Забронировать отель   8. Забронировать тур\n"
            " 9. Аренда авто          10. Экскурсии\n"
            "11. Мои брони и маршрут  12. Оплатить             13. Отменить бронь\n"
            "14. Страхование          15. Поддержка            16. Отзывы\n"
            "17. Уведомления          18. Конвертер валют      19. Аналитика\n"
            "20. Демо-сценарий целиком 21. Демонстрация исключений\n"
            " 0. Выход\n";
        while (!g_eof) {
            std::cout << menu;
            int choice = readInt("Ваш выбор: ", 0, 21);
            if (g_eof || choice == 0) break;
            try {
                switch (choice) {
                    case 1: showFlights(); break;
                    case 2: searchFlights(); break;
                    case 3: searchHotels(); break;
                    case 4: showTours(); break;
                    case 5: loginMenu(); break;
                    case 6: bookFlightMenu(); break;
                    case 7: bookHotelMenu(); break;
                    case 8: bookTourMenu(); break;
                    case 9: carMenu(); break;
                    case 10: excursionMenu(); break;
                    case 11: myBookings(); break;
                    case 12: payMenu(); break;
                    case 13: cancelMenu(); break;
                    case 14: insuranceMenu(); break;
                    case 15: supportMenu(); break;
                    case 16: reviewsMenu(); break;
                    case 17: notificationsMenu(); break;
                    case 18: currencyMenu(); break;
                    case 19: analyticsMenu(); break;
                    case 20: fullDemo(); break;
                    case 21: exceptionsDemo(); break;
                    default: break;
                }
            } catch (const TravelException& e) {
                std::cout << "Ошибка: " << e.what() << "\n";
            } catch (const std::exception& e) {
                std::cout << "Непредвиденная ошибка: " << e.what() << "\n";
            }
        }
        std::cout << "Счастливого пути!\n";
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
