
#include <iostream>
#include <windows.h>
#include "AgencyManager.h"
#include "CarRental.h"
#include "CarRentalBooking.h"
#include "ExcursionBooking.h"
#include "Itinerary.h"
#include "RefundCalculator.h"
#include "SupportOperator.h"
#include "TravelAgency.h"
#include "TravelAgent.h"
#include "TravelAnalytics.h"
#include "TravelPreferences.h"

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

const Date kToday(2026, 6, 1);

Country ru() { return Country("RU", "Россия", "RUB"); }
Country tr() {
    Country c("TR", "Турция", "TRY");
    c.addVisaFreeCountry(ru());
    return c;
}
Country fr() { return Country("FR", "Франция", "EUR"); }
City moscow() { return City("MOW", "Москва", ru(), GeoPoint(55.75, 37.61), 3); }
City antalya() { return City("AYT", "Анталья", tr(), GeoPoint(36.90, 30.71), 3); }
City paris() { return City("PAR", "Париж", fr(), GeoPoint(48.86, 2.35), 1); }
Airport svo() { return Airport("SVO", "Шереметьево", moscow(), 6); }
Airport ayt() { return Airport("AYT", "Анталья", antalya(), 2); }
Airline su() { return Airline("SU", "Аэрофлот", ru(), 4.4); }
Aircraft smallPlane() { return Aircraft("Mini", 6, 4, 10); }

Flight makeFlight(const std::string& number = "SU1", int hour = 8, int minutes = 200, Aircraft plane = smallPlane()) {
    return Flight(number, su(), svo(), ayt(), Date(2026, 9, 1), hour, minutes, plane, Money::fromUnits(10000));
}

Traveler makeTraveler(const std::string& id = "T1", Date passportExpiry = Date(2030, 1, 1)) {
    Passport passport("751234567", "Иванов Иван", ru(), passportExpiry);
    return Traveler(id, "Иван", "Иванов", ContactInfo("+7 900 123 45 67", "i@i.ru"), Date(1990, 5, 5), passport, ru());
}

Staff makeStaff(const std::string& id = "S1") {
    return Staff(id, "Анна", "Смирнова", ContactInfo("+7 900 000 00 00", "s@s.ru"), Date(1985, 1, 1), "Отдел", Money::fromUnits(50000), Date(2020, 1, 1));
}

void testDatesAndGeo() {
    CHECK(Date(2026, 10, 16).dayOfWeek() == 5);
    CHECK(Date(2026, 10, 17).isWeekend());
    CHECK(!Date(2026, 10, 19).isWeekend());
    CHECK(Date(2024, 2, 28).addDays(2).toString() == "2024-03-01");
    CHECK_THROWS(Date(2026, 2, 30), InvalidDateException);

    DateRange a(Date(2026, 7, 1), Date(2026, 7, 10));
    CHECK(a.nights() == 9);
    CHECK(a.contains(Date(2026, 7, 5)));
    CHECK(!a.contains(Date(2026, 7, 11)));
    CHECK(a.overlaps(DateRange(Date(2026, 7, 10), Date(2026, 7, 15))));
    CHECK(!a.overlaps(DateRange(Date(2026, 7, 11), Date(2026, 7, 15))));
    CHECK(a.extend(3).nights() == 12);
    CHECK_THROWS(DateRange(Date(2026, 7, 10), Date(2026, 7, 1)), InvalidDateRangeException);

    double km = moscow().distanceTo(paris());
    CHECK(km > 2400 && km < 2600);
    CHECK(moscow().timeDifferenceHours(paris()) == 2);
    CHECK(GeoPoint(10, 10).isNorthernHemisphere());
    CHECK_THROWS(GeoPoint(100, 0), InvalidDataException);
    CHECK(Money::fromUnits(100).divide(4).getCents() == 2500);
    CHECK_THROWS(Money::fromUnits(1).divide(0), InvalidDataException);
}

void testDocuments() {
    Passport p("751234567", "Иванов", ru(), Date(2027, 1, 1));
    CHECK(p.isFormatCorrect());
    CHECK(!Passport("12", "X", ru(), Date(2030, 1, 1)).isFormatCorrect());
    CHECK(p.isValidOn(Date(2026, 12, 31)));
    CHECK(p.isValidForTrip(DateRange(Date(2026, 1, 1), Date(2026, 6, 1))));
    CHECK(!p.isValidForTrip(DateRange(Date(2026, 8, 1), Date(2026, 9, 1))));
    CHECK(p.expiresSoon(Date(2026, 12, 1), 60));

    Visa visa(fr(), DateRange(Date(2026, 1, 1), Date(2026, 12, 31)), "Шенген", 1);
    CHECK(visa.coversTrip(DateRange(Date(2026, 5, 1), Date(2026, 5, 10))));
    CHECK(!visa.coversTrip(DateRange(Date(2027, 5, 1), Date(2027, 5, 10))));
    visa.useEntry();
    CHECK(visa.entriesLeft() == 0);
    CHECK_THROWS(visa.useEntry(), InvalidDocumentException);

    CHECK(tr().isVisaFreeFor(ru()));
    CHECK(!fr().isVisaFreeFor(ru()));

    Traveler t = makeTraveler();
    DateRange trip(Date(2026, 9, 1), Date(2026, 9, 10));
    CHECK(t.canEnter(tr(), trip));
    CHECK(!t.canEnter(fr(), trip));
    CHECK_THROWS(t.requireDocuments(fr(), trip), VisaRequiredException);
    t.addVisa(Visa(fr(), DateRange(Date(2026, 8, 1), Date(2026, 12, 31)), "Шенген", 2));
    CHECK(t.canEnter(fr(), trip));
    CHECK_THROWS(makeTraveler("T2", Date(2026, 9, 5)).requireDocuments(tr(), trip), InvalidDocumentException);
    CHECK(!t.isMinor(Date(2026, 6, 1)));

    TravelAdvisory adv(fr(), 4, "Опасно", kToday);
    CHECK(adv.forbidsTravel() && adv.requiresWarning() && !adv.isSafe());
    adv.relax();
    CHECK(!adv.forbidsTravel());
    CHECK_THROWS(TravelAdvisory(fr(), 9, "?", kToday), InvalidDataException);
}

void testAccounts() {
    CHECK(UserAccount::isStrongPassword("Abcdef12"));
    CHECK(!UserAccount::isStrongPassword("abcdef12"));
    CHECK(!UserAccount::isStrongPassword("Ab1"));
    CHECK_THROWS(UserAccount("user", "weak", makeTraveler()), WeakPasswordException);
    CHECK_THROWS(UserAccount("u", "Abcdef12", makeTraveler()), InvalidDataException);

    UserAccount account("ivan", "Abcdef12", makeTraveler());
    account.authenticate("Abcdef12");
    CHECK_THROWS(account.authenticate("wrong"), AuthenticationFailedException);
    CHECK_THROWS(account.authenticate("wrong"), AuthenticationFailedException);
    CHECK_THROWS(account.authenticate("wrong"), AccountLockedException);
    CHECK(account.isLocked());
    CHECK_THROWS(account.authenticate("Abcdef12"), AccountLockedException);
    account.unlock();
    account.changePassword("Abcdef12", "Newpass99");
    CHECK_THROWS(account.authenticate("Abcdef12"), AuthenticationFailedException);
    CHECK_THROWS(account.changePassword("Newpass99", "bad"), WeakPasswordException);

    LoyaltyAccount loyalty("T1", kToday);
    loyalty.earnFromPurchase(Money::fromUnits(150000));
    CHECK(loyalty.getPoints() == 1500);
    CHECK(loyalty.getTier() == LoyaltyTier::Silver);
    CHECK(loyalty.discountPercent() == 3.0);
    loyalty.redeem(1000);
    CHECK(loyalty.getTier() == LoyaltyTier::Standard);
    CHECK_THROWS(loyalty.redeem(5000), InsufficientPointsException);
    CHECK(loyalty.pointsValue().getCents() == 50000);

    TravelPreferences prefs(Money::fromUnits(20000), CabinClass::Business, 0);
    CHECK(prefs.fitsBudget(Money::fromUnits(15000)));
    CHECK(!prefs.acceptsStops(1));
    CHECK(prefs.wantsComfort());
    prefs.addFavorite(tr());
    CHECK(prefs.isFavorite(tr()) && !prefs.isFavorite(fr()));
}

void testTransport() {
    Seat seat(1, 'A', CabinClass::Economy);
    CHECK(seat.label() == "1A" && seat.isWindow() && !seat.isAisle());
    seat.reserve();
    CHECK_THROWS(seat.reserve(), NoSeatsAvailableException);
    seat.release();
    seat.reserve();

    Flight flight = makeFlight();
    CHECK(flight.availableSeats(CabinClass::Economy) == 6);
    CHECK(flight.availableSeats(CabinClass::Business) == 4);
    CHECK(flight.reserveSeat(CabinClass::Business) == "1A");
    CHECK(flight.availableSeats(CabinClass::Business) == 3);
    flight.releaseSeat("1A");
    CHECK(flight.availableSeats(CabinClass::Business) == 4);
    CHECK_THROWS(flight.releaseSeat("99Z"), InvalidDataException);
    CHECK(flight.priceFor(CabinClass::Business).getCents() == 2500000);
    CHECK(flight.isInternational());
    CHECK(flight.distanceKm() > 1500);
    CHECK(flight.occupancyPercent() == 0.0);
    CHECK(makeFlight("SU2", 23, 120).arrivalDate().toString() == "2026-09-02");
    CHECK(makeFlight("SU3", 8, 200).arrivalDate().toString() == "2026-09-01");

    Flight tiny("SU4", su(), svo(), ayt(), Date(2026, 9, 1), 8, 100, Aircraft("Tiny", 1, 0, 5), Money::fromUnits(1000));
    tiny.reserveSeat(CabinClass::Economy);
    CHECK(tiny.isFull());
    CHECK_THROWS(tiny.reserveSeat(CabinClass::Economy), NoSeatsAvailableException);
    CHECK_THROWS(Airport("sv", "x", moscow(), 1), InvalidDataException);
    CHECK_THROWS(Flight("X", su(), svo(), svo(), Date(2026, 9, 1), 8, 100, smallPlane(), Money::fromUnits(1)), InvalidDataException);

    CHECK(Aircraft("Big", 300, 28, 40).isWideBody());
    CHECK(!smallPlane().isWideBody());
    Airline airline = su();
    airline.rate(3.0);
    CHECK(!airline.isReliable());
    CHECK_THROWS(airline.rate(9.0), InvalidDataException);

    CarRental car("C1", "Corolla", antalya(), Money::fromUnits(3000), 4);
    CHECK(car.costFor(DateRange(Date(2026, 7, 1), Date(2026, 7, 4))).getCents() == 900000);
    CHECK(car.isAvailableIn(antalya()) && !car.isAvailableIn(paris()));
    car.reserve();
    CHECK_THROWS(car.reserve(), InvalidStateException);
    car.giveBack();
    CHECK(car.fitsPassengers(4) && !car.fitsPassengers(5));
}

void testHotelsAndReviews() {
    DateRange stay(Date(2026, 7, 1), Date(2026, 7, 5));
    Room room(101, RoomType::Double, 2, Money::fromUnits(5000));
    CHECK(room.isAvailable(stay));
    room.book(stay);
    CHECK(!room.isAvailable(DateRange(Date(2026, 7, 3), Date(2026, 7, 8))));
    CHECK_THROWS(room.book(DateRange(Date(2026, 7, 3), Date(2026, 7, 8))), RoomUnavailableException);
    CHECK(room.nightsBooked() == 4);
    room.cancel(stay);
    CHECK(room.isAvailable(stay));
    CHECK_THROWS(room.cancel(stay), InvalidDataException);
    CHECK(room.costFor(stay).getCents() == 2000000);
    CHECK(room.fits(2) && !room.fits(3));

    Hotel hotel("H1", "Отель", antalya(), Address("ул.", "Анталья", "1", "Турция"), 5);
    hotel.addRoom(room);
    hotel.addRoom(Room(102, RoomType::Suite, 3, Money::fromUnits(15000)));
    CHECK_THROWS(hotel.addRoom(room), DuplicateEntityException);
    CHECK(hotel.bookRoom(RoomType::Double, 2, stay) == 101);
    CHECK_THROWS(hotel.bookRoom(RoomType::Double, 2, stay), RoomUnavailableException);
    CHECK(hotel.availableRoomsCount(stay) == 1);
    hotel.cancelRoom(101, stay);
    CHECK(hotel.availableRoomsCount(stay) == 2);
    CHECK(hotel.averagePrice().getCents() == 1000000);
    CHECK(hotel.isLuxury());
    hotel.addAmenity("Бассейн");
    CHECK(hotel.hasAmenity("бассейн") && !hotel.hasAmenity("Сауна"));
    CHECK_THROWS(hotel.updateRating(11.0), InvalidDataException);
    CHECK_THROWS(Hotel("H", "x", antalya(), Address(), 7), InvalidDataException);

    ReviewBoard board;
    board.submit(Review("R1", "A", "H1", 5, "Отличный отель, всё понравилось", kToday));
    board.submit(Review("R2", "B", "H1", 2, "Шумно", kToday.addDays(-100)));
    CHECK(board.countFor("H1") == 2);
    CHECK(board.averageFor("H1") == 3.5);
    CHECK(board.positiveShare("H1") == 50.0);
    CHECK(board.recentFor("H1", kToday, 30).size() == 1);
    CHECK_THROWS(board.submit(Review("R3", "A", "H1", 4, "Повтор", kToday)), DuplicateEntityException);
    CHECK_THROWS(board.submit(Review("R4", "C", "H1", 4, "Пишите на http://x", kToday)), InvalidDataException);
    CHECK_THROWS(Review("R5", "C", "H1", 7, "?", kToday), InvalidDataException);
    CHECK(Review("R6", "C", "H1", 1, "раз два три", kToday).wordCount() == 3);
}

void testToursAndStaff() {
    Staff guideBase = makeStaff("G1");
    Guide guide(guideBase, {"русский"}, Money::fromUnits(1000));
    Attraction attraction("AT1", "Музей", antalya(), Money::fromUnits(500), 9, 18);
    CHECK(!guide.canGuide(attraction, "русский"));
    guide.certify();
    CHECK(guide.canGuide(attraction, "русский") && !guide.canGuide(attraction, "английский"));
    guide.addLanguage("английский");
    CHECK(guide.canGuide(attraction, "английский"));
    CHECK(guide.costFor(3).getCents() == 300000);
    CHECK(attraction.isOpenAt(9) && !attraction.isOpenAt(18));
    CHECK(attraction.ticketsCost(4).getCents() == 200000);
    CHECK_THROWS(Attraction("A", "x", antalya(), Money(), 18, 9), InvalidDataException);

    Excursion excursion("E1", "Музей", attraction, guide, Date(2026, 9, 3), 10, Money::fromUnits(1000));
    excursion.bookPlaces(4);
    CHECK(excursion.freePlaces() == 6);
    CHECK_THROWS(excursion.bookPlaces(7), NoSeatsAvailableException);
    excursion.cancelPlaces(2);
    CHECK(excursion.freePlaces() == 8);
    CHECK(excursion.totalCost(5).getCents() == 450000);
    CHECK(excursion.totalCost(2).getCents() == 200000);

    Hotel hotel("H1", "Отель", antalya(), Address(), 4);
    Flight out = makeFlight("SU1", 8, 200);
    Flight back("SU2", su(), ayt(), svo(), Date(2026, 9, 8), 10, 200, smallPlane(), Money::fromUnits(10000));
    TourPackage tour("T1", "Тур", antalya(), DateRange(Date(2026, 9, 1), Date(2026, 9, 8)), hotel, out, back, Money::fromUnits(50000), 3);
    CHECK(tour.durationNights() == 7);
    tour.addExcursion(excursion);
    CHECK(tour.pricePerPerson().getCents() == 5100000);
    CHECK(tour.totalPrice(2).getCents() == 10200000);
    CHECK(tour.isInternational());
    tour.reservePlaces(2);
    CHECK(tour.freePlaces() == 1);
    CHECK_THROWS(tour.reservePlaces(2), NoSeatsAvailableException);
    tour.releasePlaces(1);
    CHECK(tour.freePlaces() == 2);
    Excursion late("E2", "Поздняя", attraction, guide, Date(2026, 10, 1), 5, Money::fromUnits(100));
    CHECK_THROWS(tour.addExcursion(late), InvalidDateRangeException);
    CHECK_THROWS(TourPackage("T2", "Не тот город", paris(), DateRange(Date(2026, 9, 1), Date(2026, 9, 8)), hotel, out, back,
                             Money::fromUnits(1), 1), InvalidDataException);

    CHECK(makeStaff().yearsOfService(Date(2026, 1, 2)) == 6);
    Staff raised = makeStaff();
    raised.raiseSalary(10);
    CHECK(raised.getSalary().getCents() == 5500000);
    CHECK_THROWS(raised.raiseSalary(-1), InvalidDataException);
}

void testBookings() {
    FlightBooking fb("FL-1", "T1", kToday, Money::fromUnits(10000), "SU1", "1A", CabinClass::Economy, Date(2026, 6, 25), "SVO-AYT", "Иванов");
    CHECK(fb.type() == "Авиабилет" && fb.destination() == "AYT");
    CHECK(fb.cancellationFee(Date(2026, 6, 1)).getCents() == 100000);
    CHECK(fb.cancellationFee(Date(2026, 6, 20)).getCents() == 400000);
    CHECK(fb.cancellationFee(Date(2026, 6, 24)).getCents() == 700000);
    CHECK(fb.cancellationFee(Date(2026, 6, 25)).getCents() == 1000000);
    CHECK(fb.isCheckInOpen(Date(2026, 6, 24)) && !fb.isCheckInOpen(Date(2026, 6, 10)));
    CHECK(fb.baggageAllowanceKg() == 23);

    CHECK(fb.getStatus() == BookingStatus::Pending);
    CHECK_THROWS(fb.markPaid(), InvalidStateException);
    fb.confirm();
    fb.markPaid();
    CHECK_THROWS(fb.complete(Date(2026, 6, 1)), InvalidStateException);
    fb.complete(Date(2026, 6, 26));
    CHECK(fb.getStatus() == BookingStatus::Completed && !fb.isActive());
    CHECK_THROWS(fb.cancel(Date(2026, 6, 27)), InvalidStateException);
    CHECK_THROWS(fb.changeSeat("2B"), InvalidStateException);

    FlightBooking fb2("FL-2", "T1", kToday, Money::fromUnits(10000), "SU1", "1A", CabinClass::Business, Date(2026, 7, 25), "SVO-AYT", "И");
    fb2.confirm();
    fb2.changeSeat("2C");
    CHECK(fb2.getSeatLabel() == "2C" && fb2.baggageAllowanceKg() == 40);
    CHECK(fb2.refundAmount(kToday).getCents() == 900000);
    CHECK(fb2.cancel(kToday).getCents() == 100000);
    CHECK(fb2.getStatus() == BookingStatus::Cancelled);

    HotelBooking hb("HT-1", "T1", kToday, Money::fromUnits(20000), "H1", "Отель", 101,
                    DateRange(Date(2026, 7, 10), Date(2026, 7, 14)), 2, "Анталья");
    CHECK(hb.isExclusive() && hb.nightsCount() == 4);
    CHECK(hb.cancellationFee(Date(2026, 7, 1)).isZero());
    CHECK(hb.cancellationFee(Date(2026, 7, 9)).getCents() == 500000);
    CHECK(hb.cancellationFee(Date(2026, 7, 12)).getCents() == 2000000);
    hb.extendStay(2, Money::fromUnits(5000));
    CHECK(hb.nightsCount() == 6 && hb.getPrice().getCents() == 3000000);
    CHECK_THROWS(hb.extendStay(0, Money()), InvalidDataException);
    CHECK_THROWS(HotelBooking("X", "T1", kToday, Money(), "H", "O", 1, DateRange(Date(2026, 7, 1), Date(2026, 7, 1)), 1, "A"), InvalidDateRangeException);

    TourBooking tb("TR-1", "T1", kToday, Money::fromUnits(100000), "T1", "Тур", 4, DateRange(Date(2026, 9, 1), Date(2026, 9, 8)), "Анталья");
    CHECK(tb.cancellationFee(Date(2026, 6, 1)).getCents() == 500000);
    CHECK(tb.cancellationFee(Date(2026, 8, 20)).getCents() == 5000000);
    CHECK(tb.perPerson().getCents() == 2500000);
    tb.removeTravelers(1);
    CHECK(tb.getTravelers() == 3 && tb.getPrice().getCents() == 7500000);
    CHECK_THROWS(tb.removeTravelers(3), InvalidDataException);

    ExcursionBooking eb("EX-1", "T1", kToday, Money::fromUnits(1000), "E1", "Музей", 2, Date(2026, 9, 3));
    CHECK(eb.cancellationFee(Date(2026, 9, 1)).isZero());
    CHECK(eb.cancellationFee(Date(2026, 9, 2)).getCents() == 50000);
    CarRentalBooking cb("CR-1", "T1", kToday, Money::fromUnits(9000), "C1", "Corolla", DateRange(Date(2026, 7, 1), Date(2026, 7, 4)), "Анталья");
    CHECK(cb.cancellationFee(Date(2026, 6, 1)).isZero());
    CHECK(cb.cancellationFee(Date(2026, 7, 1)).getCents() == 300000);
    CHECK(cb.lateReturnFee(12, Money::fromUnits(3000)).getCents() == 225000);
    CHECK(cb.lateReturnFee(0, Money::fromUnits(3000)).isZero());
}

void testItineraryAndManager() {
    auto hotelA = std::make_shared<HotelBooking>("HT-1", "T1", kToday, Money::fromUnits(1000), "H1", "A", 1, DateRange(Date(2026, 7, 1), Date(2026, 7, 5)), 1, "Анталья");
    auto hotelB = std::make_shared<HotelBooking>("HT-2", "T1", kToday, Money::fromUnits(2000), "H2", "B", 1, DateRange(Date(2026, 7, 4), Date(2026, 7, 8)), 1, "Стамбул");
    auto flight = std::make_shared<FlightBooking>("FL-1", "T1", kToday, Money::fromUnits(500), "SU1", "1A", CabinClass::Economy, Date(2026, 7, 1), "SVO-AYT", "И");
    Itinerary trip("IT-1", "Лето", "T1", kToday);
    trip.addBooking(hotelA);
    trip.addBooking(flight);
    CHECK(!trip.hasConflicts());
    trip.addBooking(hotelB);
    CHECK(trip.overlappingPairs() == 1 && trip.hasConflicts());
    CHECK(trip.totalPrice().getCents() == 350000);
    CHECK(trip.totalPeriod().toString() == "2026-07-01 — 2026-07-08");
    CHECK(trip.destinations().size() == 3);
    CHECK_THROWS(trip.addBooking(hotelA), DuplicateEntityException);
    CHECK_THROWS(trip.addBooking(std::make_shared<HotelBooking>("HT-9", "OTHER", kToday, Money(), "H", "A", 1, DateRange(Date(2026, 7, 1), Date(2026, 7, 2)), 1, "X")), AccessDeniedException);
    trip.removeBooking("HT-2");
    CHECK(!trip.hasConflicts());
    CHECK_THROWS(trip.removeBooking("HT-2"), BookingNotFoundException);
    CHECK(!trip.isFullyPaid());
    CHECK_THROWS(Itinerary("E", "Пусто", "T1", kToday).totalPeriod(), InvalidStateException);

    BookingManager manager;
    CHECK(manager.generateId("FL") == "FL-1" && manager.generateId("FL") == "FL-2");
    manager.add(flight);
    manager.add(hotelA);
    CHECK_THROWS(manager.add(flight), DuplicateEntityException);
    CHECK_THROWS(manager.find("NOPE"), BookingNotFoundException);
    CHECK(manager.activeFor("T1").size() == 2);
    flight->confirm();
    CHECK(manager.cancel("FL-1", Date(2026, 6, 1)).getCents() == 5000);
    CHECK(manager.countByStatus(BookingStatus::Cancelled) == 1);
    CHECK(manager.getCancelledFees().getCents() == 5000);
    CHECK(manager.revenue().isZero());
}

void testPaymentsAndPromo() {
    Payment payment(1, "B1", Money::fromUnits(1000), PaymentMethod::Card, kToday);
    CHECK_THROWS(payment.capture(), InvalidStateException);
    payment.authorize();
    payment.capture();
    CHECK(payment.isSuccessful());
    payment.refund(Money::fromUnits(400));
    CHECK(payment.refundableAmount().getCents() == 60000 && payment.isSuccessful());
    CHECK_THROWS(payment.refund(Money::fromUnits(700)), PaymentDeclinedException);
    payment.refund(Money::fromUnits(600));
    CHECK(payment.getStatus() == PaymentStatus::Refunded);
    CHECK(Payment(2, "B", Money::fromUnits(1000), PaymentMethod::Cash, kToday).fee(2.5).getCents() == 2500);
    CHECK_THROWS(Payment(3, "B", Money(), PaymentMethod::Card, kToday).authorize(), PaymentDeclinedException);

    Booking booking("B1", "T1", kToday, Money::fromUnits(5000));
    PaymentProcessor processor(2.0, Money::fromUnits(10000));
    Payment charged = processor.charge(booking, Money::fromUnits(5000), PaymentMethod::Card, kToday);
    CHECK(charged.isSuccessful() && processor.successfulCount() == 1);
    CHECK_THROWS(processor.charge(booking, Money::fromUnits(20000), PaymentMethod::Card, kToday), PaymentDeclinedException);
    processor.refund(charged.getId(), Money::fromUnits(1000));
    CHECK(processor.totalCollected().getCents() == 400000 && processor.totalRefunded().getCents() == 100000);
    CHECK(processor.feeFor(Money::fromUnits(1000)).getCents() == 2000);
    CHECK(processor.paymentsFor("B1").size() == 1 && processor.paymentsFor("B2").empty());
    CHECK_THROWS(processor.findPayment(99), InvalidDataException);

    PromoCode promo("SALE", 20.0, DateRange(Date(2026, 5, 1), Date(2026, 7, 1)), Money::fromUnits(1000), 1);
    CHECK(promo.applyTo(Money::fromUnits(5000), kToday).getCents() == 400000);
    CHECK_THROWS(promo.applyTo(Money::fromUnits(500), kToday), PromoCodeInvalidException);
    CHECK_THROWS(promo.applyTo(Money::fromUnits(5000), Date(2026, 8, 1)), PromoCodeInvalidException);
    promo.registerUse();
    CHECK(promo.remainingUses() == 0 && !promo.isValidOn(kToday));
    CHECK_THROWS(PromoCode("X", 95.0, DateRange(kToday, kToday), Money(), 1), InvalidDataException);
}

void testInsuranceAndSupport() {
    CHECK(InsurancePolicy::premiumFor(Money::fromUnits(10000), InsuranceType::Basic).getCents() == 30000);
    CHECK(InsurancePolicy::premiumFor(Money::fromUnits(10000), InsuranceType::Premium).getCents() == 80000);
    InsurancePolicy policy("P1", "B1", InsuranceType::Extended, Money::fromUnits(10000), Money::fromUnits(500),
                           DateRange(Date(2026, 6, 1), Date(2026, 6, 30)));
    CHECK(policy.covers("болезнь") && policy.covers("отмена рейса") && !policy.covers("кража"));
    policy.fileClaim(InsuranceClaim("C1", Money::fromUnits(4000), "болезнь", kToday), kToday);
    policy.approveClaim("C1");
    CHECK(policy.remainingCoverage().getCents() == 600000 && policy.totalPaidOut().getCents() == 400000);
    CHECK_THROWS(policy.fileClaim(InsuranceClaim("C2", Money::fromUnits(9000), "болезнь", kToday), kToday), InsuranceClaimException);
    CHECK_THROWS(policy.fileClaim(InsuranceClaim("C3", Money::fromUnits(10), "кража", kToday), kToday), InsuranceClaimException);
    CHECK_THROWS(policy.fileClaim(InsuranceClaim("C4", Money::fromUnits(10), "болезнь", Date(2026, 8, 1)), Date(2026, 8, 1)), InsuranceClaimException);
    CHECK_THROWS(policy.approveClaim("ZZ"), InsuranceClaimException);
    InsuranceClaim claim("C5", Money::fromUnits(500), "болезнь", kToday);
    CHECK(claim.isPending() && claim.payout(Money::fromUnits(200)).getCents() == 20000);
    claim.reject();
    CHECK_THROWS(claim.approve(), InvalidStateException);

    NotificationService service(30);
    service.send("T1", "Ваш билет подтверждён и ждёт вас в личном кабинете, хорошей поездки", Date(2026, 1, 1));
    service.send("T1", "Скидка на отели", kToday);
    service.send("T2", "Привет", kToday);
    CHECK(service.countFor("T1") == 2 && service.unreadFor("T1").size() == 2);
    service.markAllRead("T1");
    CHECK(service.unreadFor("T1").empty());
    CHECK(service.purgeOld(kToday) == 1 && service.countFor("T1") == 1);
    CHECK_THROWS(service.send("T1", "", kToday), InvalidDataException);
    CHECK(Notification(1, "T", "Очень длинное сообщение для проверки", kToday).preview(10).size() > 0);

    SupportTicket ticket(1, "T1", "B1", "Вопрос", Date(2026, 5, 20));
    CHECK(ticket.isOpen() && ticket.ageDays(kToday) == 12 && ticket.isStale(kToday, 10));
    CHECK_THROWS(ticket.resolve(), InvalidStateException);
    SupportOperator op(makeStaff("S2"), 1);
    op.takeTicket(ticket);
    CHECK(op.isOverloaded() && op.workloadPercent() == 100.0);
    SupportTicket second(2, "T1", "", "Ещё", kToday);
    CHECK_THROWS(op.takeTicket(second), AccessDeniedException);
    op.replyTo(ticket, "Решаем");
    op.resolveTicket(ticket);
    CHECK(ticket.getMessages().size() == 1 && op.getResolvedTotal() == 1 && !op.isOverloaded());
    ticket.reopen();
    ticket.resolve();
    ticket.close();
    CHECK_THROWS(ticket.addMessage("x", "y"), InvalidStateException);
    CHECK_THROWS(SupportTicket(3, "T", "", "", kToday), InvalidDataException);

    AgencyManager manager(makeStaff("S3"), Money::fromUnits(10000));
    CHECK(manager.canApproveRefund(Money::fromUnits(10000)) && !manager.canApproveRefund(Money::fromUnits(10001)));
    manager.approveRefund(Money::fromUnits(5000));
    CHECK_THROWS(manager.approveRefund(Money::fromUnits(50000)), AccessDeniedException);
    CHECK(manager.canRefund() && manager.approveDiscount(30) && !manager.approveDiscount(31));
    UserAccount locked("locked", "Abcdef12", makeTraveler());
    for (int i = 0; i < 3; ++i) {
        try { locked.authenticate("bad"); } catch (const TravelException&) {}
    }
    manager.unlockAccount(locked);
    CHECK(!locked.isLocked());
    CHECK(manager.role() == "Руководитель агентства");

    TravelAgent agent(makeStaff("S4"), 5.0, {"русский"});
    CHECK(agent.commissionFor(Money::fromUnits(10000)).getCents() == 50000 && agent.speaks("русский") && !agent.speaks("французский"));
    Booking unpaid("B", "T1", kToday, Money::fromUnits(1000));
    CHECK_THROWS(agent.registerSale(unpaid), InvalidStateException);
    unpaid.confirm();
    unpaid.markPaid();
    CHECK(agent.registerSale(unpaid).getCents() == 5000 && agent.getSalesCount() == 1);
    CHECK(agent.monthlyBonus().getCents() == 30000);
}

void testPricingAndAnalytics() {
    PriceCalculator calc;
    Money price = Money::fromUnits(1000);
    CHECK(calc.isHighSeason(Date(2026, 7, 1)) && !calc.isHighSeason(Date(2026, 3, 1)));
    CHECK(calc.seasonalPrice(price, Date(2026, 7, 1)).getCents() == 130000);
    CHECK(calc.weekendPrice(price, Date(2026, 10, 17)).getCents() == 110000);
    CHECK(calc.weekendPrice(price, Date(2026, 10, 19)).getCents() == 100000);
    CHECK(calc.groupDiscount(price, 4).getCents() == 92000 && calc.groupDiscount(price, 2).getCents() == 100000);
    CHECK(calc.totalForStay(price, DateRange(Date(2026, 3, 2), Date(2026, 3, 4)), 1).getCents() == 200000);

    CurrencyConverter converter("RUB");
    converter.setRate("USD", 90.0);
    CHECK(converter.supports("USD") && !converter.supports("EUR"));
    CHECK(converter.convert(Money::fromUnits(9000), "USD").getCents() == 10000);
    CHECK(converter.convert(Money::fromUnits(100, "USD"), "RUB").getCents() == 900000);
    CHECK_THROWS(converter.convert(Money::fromUnits(1), "EUR"), InvalidDataException);
    CHECK_THROWS(converter.setRate("EUR", 0.0), InvalidDataException);
    CHECK(converter.rateOf("USD") == 90.0);

    RefundCalculator refunds(Money::fromUnits(100), Money::fromUnits(50), 14);
    auto booking = std::make_shared<FlightBooking>("FL-1", "T1", kToday, Money::fromUnits(10000), "SU1", "1A", CabinClass::Economy, Date(2026, 9, 1), "SVO-AYT", "И");
    CHECK(refunds.refundFor(*booking, kToday).isZero());
    booking->confirm();
    CHECK(refunds.isRefundable(*booking, kToday) && refunds.isWithinWindow(*booking, kToday));
    CHECK(refunds.refundFor(*booking, kToday).getCents() == 890000);
    CHECK(refunds.refundSharePercent(*booking, kToday) == 89.0);
    CHECK(!refunds.isRefundable(*booking, Date(2026, 9, 5)));

    auto cancelled = std::make_shared<FlightBooking>("FL-2", "T1", kToday, Money::fromUnits(2000), "SU1", "2A", CabinClass::Economy, Date(2026, 9, 1), "SVO-CDG", "И");
    cancelled->confirm();
    cancelled->cancel(kToday);
    std::vector<std::shared_ptr<Booking>> all = {booking, cancelled};
    booking->markPaid();
    TravelAnalytics analytics("Тест", kToday);
    CHECK(analytics.revenue(all).getCents() == 1000000);
    CHECK(analytics.cancellationRate(all) == 50.0);
    CHECK(analytics.averageBookingValue(all).getCents() == 1000000);
    CHECK(analytics.countByType(all, "Авиабилет") == 2);
    CHECK(analytics.mostPopularDestination(all) == "AYT");
    CHECK(analytics.summary(all).find("Бронирований: 2") != std::string::npos);
    CHECK(analytics.cancellationRate({}) == 0.0);
}

void testAgencyEndToEnd() {
    TravelAgency agency("Тест", CurrencyConverter("RUB"), PriceCalculator());
    agency.getCatalog().addFlight(makeFlight("SU1", 8, 200));
    agency.getCatalog().addFlight(Flight("SU9", su(), svo(), Airport("KEF", "Кефлавик", City("REK", "Рейкьявик", Country("IS", "Исландия", "ISK"), GeoPoint(64, -21), 0), 1),
                                         Date(2026, 9, 1), 7, 300, smallPlane(), Money::fromUnits(30000)));
    agency.getCatalog().addAdvisory(TravelAdvisory(Country("IS", "Исландия", "ISK"), 4, "Вулкан", kToday));
    agency.registerAccount(UserAccount("ivan", "Abcdef12", makeTraveler("T1")), kToday);
    CHECK_THROWS(agency.registerAccount(UserAccount("ivan", "Abcdef12", makeTraveler("T1")), kToday), DuplicateEntityException);
    CHECK_THROWS(agency.login("ivan", "bad"), AuthenticationFailedException);
    CHECK_THROWS(agency.login("nobody", "Abcdef12"), AuthenticationFailedException);
    agency.login("ivan", "Abcdef12");

    std::string id = agency.bookFlight("ivan", "SU1", CabinClass::Economy, kToday);
    CHECK(agency.getBookings().find(id)->getStatus() == BookingStatus::Confirmed);
    CHECK(agency.getCatalog().findFlight("SU1").availableSeats(CabinClass::Economy) == 5);
    CHECK_THROWS(agency.bookFlight("ivan", "SU9", CabinClass::Economy, kToday), TravelRestrictionException);
    CHECK_THROWS(agency.bookFlight("ivan", "NOPE", CabinClass::Economy, kToday), FlightNotFoundException);

    agency.addPromo(PromoCode("HALF", 50.0 / 2, DateRange(Date(2026, 5, 1), Date(2026, 7, 1)), Money(), 5));
    Payment payment = agency.pay(id, PaymentMethod::Card, "HALF", kToday);
    CHECK(payment.getAmount().getCents() == 750000);
    CHECK(agency.loyaltyFor("T1").getPoints() == 75);
    CHECK_THROWS(agency.pay(id, PaymentMethod::Card, "", kToday), InvalidStateException);
    CHECK_THROWS(agency.pay(id, PaymentMethod::Card, "NOPE", kToday), InvalidStateException);

    InsurancePolicy& policy = agency.buyInsurance(id, InsuranceType::Basic, kToday);
    CHECK(policy.getPremium().getCents() == 30000);
    Money refund = agency.cancelBooking(id, kToday);
    CHECK(refund.getCents() == 650000);
    CHECK(agency.getCatalog().findFlight("SU1").availableSeats(CabinClass::Economy) == 6);
    CHECK(agency.getPayments().totalRefunded().getCents() == 650000);
    CHECK_THROWS(agency.cancelBooking(id, kToday), InvalidStateException);
    CHECK_THROWS(agency.buyInsurance(id, InsuranceType::Basic, kToday), InvalidStateException);

    int ticket = agency.openTicket("ivan", id, "Вернули не всё", kToday);
    CHECK(agency.findTicket(ticket).isOpen());
    CHECK_THROWS(agency.findTicket(99), InvalidDataException);
    CHECK_THROWS(agency.openTicket("ivan", "ZZ-9", "?", kToday), BookingNotFoundException);
    agency.addReview("ivan", "SU1", 5, "Отличный перелёт", kToday);
    CHECK(agency.getReviews().averageFor("SU1") == 5.0);
    CHECK(agency.convertPrice(Money::fromUnits(100), "RUB").getCents() == 10000);
    CHECK(agency.getNotifications().countFor("T1") >= 4);

    agency.registerAccount(UserAccount("maria", "Abcdef12", makeTraveler("T2", Date(2026, 9, 5))), kToday);
    CHECK_THROWS(agency.bookFlight("maria", "SU1", CabinClass::Economy, kToday), InvalidDocumentException);
}

}  // namespace

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
    testDatesAndGeo();
    testDocuments();
    testAccounts();
    testTransport();
    testHotelsAndReviews();
    testToursAndStaff();
    testBookings();
    testItineraryAndManager();
    testPaymentsAndPromo();
    testInsuranceAndSupport();
    testPricingAndAnalytics();
    testAgencyEndToEnd();
    std::cout << "Проверок: " << g_total << ", провалено: " << g_failed << "\n";
    return g_failed == 0 ? 0 : 1;
}
