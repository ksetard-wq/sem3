#pragma once
#include <stdexcept>
#include <string>

class WarehouseException : public std::runtime_error {
public:
    explicit WarehouseException(const std::string& message) : std::runtime_error(message) {}
};

class InvalidISBNException : public WarehouseException {
public:
    explicit InvalidISBNException(const std::string& code)
        : WarehouseException("Некорректный ISBN: " + code) {}
};

class InvalidDateException : public WarehouseException {
public:
    explicit InvalidDateException(const std::string& text)
        : WarehouseException("Некорректная дата: " + text) {}
};

class InvalidDataException : public WarehouseException {
public:
    explicit InvalidDataException(const std::string& details)
        : WarehouseException("Некорректные данные: " + details) {}
};

class BookNotFoundException : public WarehouseException {
public:
    explicit BookNotFoundException(const std::string& key)
        : WarehouseException("Книга не найдена: " + key) {}
};

class InsufficientStockException : public WarehouseException {
    std::string isbn_;
    int requested_;
    int available_;

public:
    InsufficientStockException(const std::string& isbn, int requested, int available)
        : WarehouseException("Недостаточно товара " + isbn + ": запрошено " + std::to_string(requested) +
                             ", доступно " + std::to_string(available)),
          isbn_(isbn), requested_(requested), available_(available) {}
    const std::string& getIsbn() const { return isbn_; }
    int getRequested() const { return requested_; }
    int getAvailable() const { return available_; }
};

class ShelfOverloadException : public WarehouseException {
public:
    explicit ShelfOverloadException(const std::string& placeId)
        : WarehouseException("Превышена допустимая нагрузка или нет места: " + placeId) {}
};

class ClimateViolationException : public WarehouseException {
public:
    ClimateViolationException(const std::string& zoneId, double temperature, double humidity)
        : WarehouseException("Нарушен климат в зоне " + zoneId + " (t=" + std::to_string(temperature) +
                             ", влажность=" + std::to_string(humidity) + ")") {}
};

class OrderNotFoundException : public WarehouseException {
public:
    explicit OrderNotFoundException(int id)
        : WarehouseException("Заказ не найден: №" + std::to_string(id)) {}
};

class InvalidOrderStateException : public WarehouseException {
public:
    InvalidOrderStateException(const std::string& action, const std::string& state)
        : WarehouseException("Действие \"" + action + "\" недопустимо в состоянии \"" + state + "\"") {}
};

class PaymentFailedException : public WarehouseException {
public:
    explicit PaymentFailedException(const std::string& reason)
        : WarehouseException("Ошибка платежа: " + reason) {}
};

class DiscountExpiredException : public WarehouseException {
public:
    explicit DiscountExpiredException(const std::string& code)
        : WarehouseException("Скидка недействительна: " + code) {}
};

class SupplierNotFoundException : public WarehouseException {
public:
    explicit SupplierNotFoundException(const std::string& key)
        : WarehouseException("Поставщик не найден: " + key) {}
};

class AccessDeniedException : public WarehouseException {
public:
    AccessDeniedException(const std::string& who, const std::string& action)
        : WarehouseException("Доступ запрещён: " + who + " не может выполнить \"" + action + "\"") {}
};

class DuplicateEntityException : public WarehouseException {
public:
    explicit DuplicateEntityException(const std::string& what)
        : WarehouseException("Дубликат: " + what) {}
};
