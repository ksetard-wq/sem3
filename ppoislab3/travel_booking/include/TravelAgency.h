#pragma once
#include <memory>
#include <string>
#include <vector>
#include "BookingManager.h"
#include "CurrencyConverter.h"
#include "FlightBooking.h"
#include "HotelBooking.h"
#include "InsurancePolicy.h"
#include "LoyaltyAccount.h"
#include "NotificationService.h"
#include "PaymentProcessor.h"
#include "PriceCalculator.h"
#include "PromoCode.h"
#include "ReviewBoard.h"
#include "SupportTicket.h"
#include "TourBooking.h"
#include "TravelCatalog.h"
#include "UserAccount.h"

// Фасад всей системы бронирования: связывает каталог, аккаунты, брони, платежи и поддержку.
class TravelAgency {
    std::string name_;
    TravelCatalog catalog_;
    std::vector<UserAccount> accounts_;
    std::vector<LoyaltyAccount> loyalty_;
    BookingManager bookings_;
    PaymentProcessor payments_;
    NotificationService notifications_;
    ReviewBoard reviews_;
    std::vector<PromoCode> promoCodes_;
    CurrencyConverter converter_;
    PriceCalculator pricing_;
    std::vector<SupportTicket> tickets_;
    std::vector<InsurancePolicy> insurance_;
    int nextTicketId_ = 1;
    int nextPolicyId_ = 1;

public:
    TravelAgency(std::string name, CurrencyConverter converter, PriceCalculator pricing)
        : name_(std::move(name)), converter_(std::move(converter)), pricing_(std::move(pricing)) {}

    // ------------------------------------------------------------ аккаунты

    void registerAccount(const UserAccount& account, const Date& today) {
        for (const auto& a : accounts_) {
            if (a.getLogin() == account.getLogin()) throw DuplicateEntityException("логин " + account.getLogin());
        }
        accounts_.push_back(account);
        loyalty_.emplace_back(account.getTraveler().getId(), today);
        notifications_.send(account.getTraveler().getId(), "Добро пожаловать в " + name_ + "!", today);
    }

    UserAccount& findAccount(const std::string& login) {
        for (auto& a : accounts_) {
            if (a.getLogin() == login) return a;
        }
        throw AuthenticationFailedException(login);
    }

    UserAccount& login(const std::string& login, const std::string& password) {
        UserAccount& account = findAccount(login);
        account.authenticate(password);
        return account;
    }

    LoyaltyAccount& loyaltyFor(const std::string& travelerId) {
        for (auto& l : loyalty_) {
            if (l.getTravelerId() == travelerId) return l;
        }
        throw InvalidDataException("бонусный счёт не найден: " + travelerId);
    }

    // ------------------------------------------------------------ проверки

    void checkAdvisory(const Country& country) const {
        const TravelAdvisory* advisory = catalog_.advisoryFor(country);
        if (advisory != nullptr && advisory->forbidsTravel()) throw TravelRestrictionException(country.getName(), advisory->getMessage());
    }

    void checkDocuments(const Traveler& traveler, const Country& destination, const DateRange& trip) const {
        checkAdvisory(destination);
        if (!destination.isSameCountry(traveler.getCitizenship())) traveler.requireDocuments(destination, trip);
    }

    // ---------------------------------------------------------- бронирование

    std::string bookFlight(const std::string& login, const std::string& flightNumber, CabinClass cabin, const Date& today) {
        UserAccount& account = findAccount(login);
        Flight& flight = catalog_.findFlight(flightNumber);
        const Country& country = flight.getDestination().getCity().getCountry();
        checkDocuments(account.getTraveler(), country, DateRange(flight.getDeparture(), flight.arrivalDate()));
        Money price = pricing_.seasonalPrice(flight.priceFor(cabin), flight.getDeparture());
        std::string seat = flight.reserveSeat(cabin);
        auto booking = std::make_shared<FlightBooking>(
            bookings_.generateId("FL"), account.getTraveler().getId(), today, price, flightNumber, seat, cabin,
            flight.getDeparture(), flight.getOrigin().getCode() + "-" + flight.getDestination().getCode(),
            account.getTraveler().fullName());
        booking->confirm();
        bookings_.add(booking);
        notifications_.notifyBooking(*booking, "подтверждено, место " + seat, today);
        return booking->getId();
    }

    std::string bookHotel(const std::string& login, const std::string& hotelId, RoomType type, const DateRange& stay,
                          int guests, const Date& today) {
        UserAccount& account = findAccount(login);
        Hotel& hotel = catalog_.findHotel(hotelId);
        checkDocuments(account.getTraveler(), hotel.getCity().getCountry(), stay);
        Room* room = hotel.findRoom(type, guests, stay);
        if (room == nullptr) throw RoomUnavailableException(hotel.getName() + ": нет свободного номера на " + stay.toString());
        Money price = pricing_.totalForStay(room->getPricePerNight(), stay, guests);
        int number = hotel.bookRoom(type, guests, stay);
        auto booking = std::make_shared<HotelBooking>(bookings_.generateId("HT"), account.getTraveler().getId(), today, price,
                                                      hotel.getId(), hotel.getName(), number, stay, guests, hotel.getCity().getName());
        booking->confirm();
        bookings_.add(booking);
        notifications_.notifyBooking(*booking, "подтверждено, номер " + std::to_string(number), today);
        return booking->getId();
    }

    std::string bookTour(const std::string& login, const std::string& tourId, int travelers, const Date& today) {
        UserAccount& account = findAccount(login);
        TourPackage& tour = catalog_.findTour(tourId);
        checkDocuments(account.getTraveler(), tour.getDestination().getCountry(), tour.getPeriod());
        Money price = pricing_.groupDiscount(tour.totalPrice(travelers), travelers);
        tour.reservePlaces(travelers);
        auto booking = std::make_shared<TourBooking>(bookings_.generateId("TR"), account.getTraveler().getId(), today, price,
                                                     tour.getId(), tour.getTitle(), travelers, tour.getPeriod(),
                                                     tour.getDestination().getName());
        booking->confirm();
        bookings_.add(booking);
        notifications_.notifyBooking(*booking, "подтверждено", today);
        return booking->getId();
    }

    // ----------------------------------------------------- оплата и отмена

    PromoCode& findPromo(const std::string& code) {
        for (auto& p : promoCodes_) {
            if (p.getCode() == code) return p;
        }
        throw PromoCodeInvalidException(code);
    }

    void addPromo(const PromoCode& promo) { promoCodes_.push_back(promo); }

    Payment pay(const std::string& bookingId, PaymentMethod method, const std::string& promoCode, const Date& today) {
        std::shared_ptr<Booking> booking = bookings_.find(bookingId);
        booking->requireStatus(BookingStatus::Confirmed, "оплатить бронирование");
        Money amount = booking->getPrice();
        LoyaltyAccount& loyalty = loyaltyFor(booking->getTravelerId());
        amount = amount.percentOf(100.0 - loyalty.discountPercent());
        if (!promoCode.empty()) {
            PromoCode& promo = findPromo(promoCode);
            amount = promo.applyTo(amount, today);
            promo.registerUse();
        }
        Payment payment = payments_.charge(*booking, amount, method, today);
        booking->markPaid();
        loyalty.earnFromPurchase(amount);
        notifications_.notifyBooking(*booking, "оплачено " + amount.toString(), today);
        return payment;
    }

    void releaseResources(const Booking& booking) {
        if (const auto* fb = dynamic_cast<const FlightBooking*>(&booking)) {
            catalog_.findFlight(fb->getFlightNumber()).releaseSeat(fb->getSeatLabel());
        } else if (const auto* hb = dynamic_cast<const HotelBooking*>(&booking)) {
            catalog_.findHotel(hb->getHotelId()).cancelRoom(hb->getRoomNumber(), hb->getStay());
        } else if (const auto* tb = dynamic_cast<const TourBooking*>(&booking)) {
            catalog_.findTour(tb->getTourId()).releasePlaces(tb->getTravelers());
        }
    }

    // Отмена брони: возвращает сумму, возвращённую клиенту.
    Money cancelBooking(const std::string& bookingId, const Date& today) {
        std::shared_ptr<Booking> booking = bookings_.find(bookingId);
        bool wasPaid = booking->getStatus() == BookingStatus::Paid;
        Money fee = bookings_.cancel(bookingId, today);
        releaseResources(*booking);
        Money refund;
        if (wasPaid) {
            for (const auto& p : payments_.paymentsFor(bookingId)) {
                if (!p.isSuccessful()) continue;
                refund = p.getAmount().subtract(fee);
                if (refund.isNegative()) refund = Money();
                if (refund.isGreaterThan(p.refundableAmount())) refund = p.refundableAmount();
                if (!refund.isZero()) payments_.refund(p.getId(), refund);
                break;
            }
        }
        notifications_.notifyBooking(*booking, "отменено, удержано " + fee.toString() + ", возврат " + refund.toString(), today);
        return refund;
    }

    // ----------------------------------------------- страхование и поддержка

    InsurancePolicy& buyInsurance(const std::string& bookingId, InsuranceType type, const Date& today) {
        std::shared_ptr<Booking> booking = bookings_.find(bookingId);
        if (!booking->isActive()) throw InvalidStateException("купить страховку", toString(booking->getStatus()));
        Money premium = InsurancePolicy::premiumFor(booking->getPrice(), type);
        DateRange period(today, booking->period().getEnd());
        insurance_.emplace_back("INS-" + std::to_string(nextPolicyId_++), bookingId, type, booking->getPrice(), premium, period);
        return insurance_.back();
    }

    int openTicket(const std::string& login, const std::string& bookingId, const std::string& subject, const Date& today) {
        UserAccount& account = findAccount(login);
        if (!bookingId.empty()) bookings_.find(bookingId);
        tickets_.emplace_back(nextTicketId_, account.getTraveler().getId(), bookingId, subject, today);
        return nextTicketId_++;
    }

    SupportTicket& findTicket(int id) {
        for (auto& t : tickets_) {
            if (t.getId() == id) return t;
        }
        throw InvalidDataException("обращение не найдено: " + std::to_string(id));
    }

    void addReview(const std::string& login, const std::string& targetId, int rating, const std::string& text, const Date& today) {
        UserAccount& account = findAccount(login);
        std::string id = "RV-" + std::to_string(reviews_.getReviews().size() + 1);
        reviews_.submit(Review(id, account.getTraveler().getId(), targetId, rating, text, today));
    }

    Money convertPrice(const Money& amount, const std::string& currency) const { return converter_.convert(amount, currency); }

    TravelCatalog& getCatalog() { return catalog_; }
    const TravelCatalog& getCatalog() const { return catalog_; }
    BookingManager& getBookings() { return bookings_; }
    const BookingManager& getBookings() const { return bookings_; }
    PaymentProcessor& getPayments() { return payments_; }
    NotificationService& getNotifications() { return notifications_; }
    ReviewBoard& getReviews() { return reviews_; }
    const std::vector<PromoCode>& getPromoCodes() const { return promoCodes_; }
    const std::vector<SupportTicket>& getTickets() const { return tickets_; }
    const std::vector<InsurancePolicy>& getInsurance() const { return insurance_; }
    const std::vector<UserAccount>& getAccounts() const { return accounts_; }
    const PriceCalculator& getPricing() const { return pricing_; }
    const CurrencyConverter& getConverter() const { return converter_; }
    const std::string& getName() const { return name_; }
};
