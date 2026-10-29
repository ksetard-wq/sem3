#pragma once
#include <stdexcept>
#include <string>

class TravelException : public std::runtime_error {
public:
    explicit TravelException(const std::string& message) : std::runtime_error(message) {}
};

class InvalidDateException : public TravelException {
public:
    explicit InvalidDateException(const std::string& text) : TravelException("Некорректная дата: " + text) {}
};

class InvalidDateRangeException : public TravelException {
public:
    explicit InvalidDateRangeException(const std::string& details)
        : TravelException("Некорректный период: " + details) {}
};

class InvalidDataException : public TravelException {
public:
    explicit InvalidDataException(const std::string& details) : TravelException("Некорректные данные: " + details) {}
};

class InvalidDocumentException : public TravelException {
public:
    explicit InvalidDocumentException(const std::string& details) : TravelException("Проблема с документом: " + details) {}
};

class VisaRequiredException : public TravelException {
public:
    explicit VisaRequiredException(const std::string& country)
        : TravelException("Для въезда в страну \"" + country + "\" нужна действующая виза") {}
};

class FlightNotFoundException : public TravelException {
public:
    explicit FlightNotFoundException(const std::string& key) : TravelException("Рейс не найден: " + key) {}
};

class NoSeatsAvailableException : public TravelException {
    int requested_;
    int available_;

public:
    NoSeatsAvailableException(const std::string& what, int requested, int available)
        : TravelException("Недостаточно мест (" + what + "): нужно " + std::to_string(requested) + ", свободно " +
                          std::to_string(available)),
          requested_(requested), available_(available) {}
    int getRequested() const { return requested_; }
    int getAvailable() const { return available_; }
};

class RoomUnavailableException : public TravelException {
public:
    explicit RoomUnavailableException(const std::string& details)
        : TravelException("Номер недоступен: " + details) {}
};

class BookingNotFoundException : public TravelException {
public:
    explicit BookingNotFoundException(const std::string& id) : TravelException("Бронирование не найдено: " + id) {}
};

class InvalidStateException : public TravelException {
public:
    InvalidStateException(const std::string& action, const std::string& state)
        : TravelException("Действие \"" + action + "\" недопустимо в состоянии \"" + state + "\"") {}
};

class PaymentDeclinedException : public TravelException {
public:
    explicit PaymentDeclinedException(const std::string& reason) : TravelException("Платёж отклонён: " + reason) {}
};

class InsufficientPointsException : public TravelException {
public:
    InsufficientPointsException(int requested, int available)
        : TravelException("Недостаточно бонусных баллов: нужно " + std::to_string(requested) + ", есть " +
                          std::to_string(available)) {}
};

class PromoCodeInvalidException : public TravelException {
public:
    explicit PromoCodeInvalidException(const std::string& code)
        : TravelException("Промокод недействителен: " + code) {}
};

class AccountLockedException : public TravelException {
public:
    explicit AccountLockedException(const std::string& login)
        : TravelException("Аккаунт заблокирован: " + login) {}
};

class AuthenticationFailedException : public TravelException {
public:
    explicit AuthenticationFailedException(const std::string& login)
        : TravelException("Неверный логин или пароль: " + login) {}
};

class WeakPasswordException : public TravelException {
public:
    WeakPasswordException()
        : TravelException("Слабый пароль: нужно минимум 8 символов, заглавная и строчная буквы и цифра") {}
};

class TravelRestrictionException : public TravelException {
public:
    TravelRestrictionException(const std::string& country, const std::string& message)
        : TravelException("Поездка в \"" + country + "\" ограничена: " + message) {}
};

class InsuranceClaimException : public TravelException {
public:
    explicit InsuranceClaimException(const std::string& reason)
        : TravelException("Страховой случай не принят: " + reason) {}
};

class DuplicateEntityException : public TravelException {
public:
    explicit DuplicateEntityException(const std::string& what) : TravelException("Дубликат: " + what) {}
};

class AccessDeniedException : public TravelException {
public:
    AccessDeniedException(const std::string& who, const std::string& action)
        : TravelException("Доступ запрещён: " + who + " не может выполнить \"" + action + "\"") {}
};
