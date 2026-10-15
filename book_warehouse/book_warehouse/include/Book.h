#pragma once
#include <string>
#include <utility>
#include "Author.h"
#include "Genre.h"
#include "ISBN.h"
#include "Money.h"
#include "Publisher.h"
#include "utils.h"

class Book {
    ISBN isbn_;
    std::string title_;
    Author author_;
    Publisher publisher_;
    Genre genre_;
    Money price_;
    int weightGrams_ = 0;
    int pages_ = 0;
    int publicationYear_ = 0;

public:
    Book(ISBN isbn, std::string title, Author author, Publisher publisher, Genre genre, Money price,
         int weightGrams, int pages, int publicationYear)
        : isbn_(std::move(isbn)), title_(std::move(title)), author_(std::move(author)),
          publisher_(std::move(publisher)), genre_(std::move(genre)), price_(std::move(price)),
          weightGrams_(weightGrams), pages_(pages), publicationYear_(publicationYear) {
        if (title_.empty() || weightGrams_ <= 0 || pages_ <= 0) {
            throw InvalidDataException("книга \"" + title_ + "\"");
        }
    }

    bool isOldEdition(int currentYear, int years) const { return currentYear - publicationYear_ >= years; }
    Money discountedPrice(double percent) const { return price_.percentOf(100.0 - percent); }
    double weightKg() const { return weightGrams_ / 1000.0; }
    bool isSuitableForAge(int age) const { return genre_.isSuitableForAge(age); }

    bool matchesQuery(const std::string& query) const {
        return containsIgnoreCase(title_, query) || containsIgnoreCase(author_.getName(), query);
    }

    void changePrice(const Money& newPrice) {
        if (newPrice.isNegative() || newPrice.isZero()) throw InvalidDataException("цена должна быть положительной");
        price_ = newPrice;
    }

    const ISBN& getIsbn() const { return isbn_; }
    const std::string& getTitle() const { return title_; }
    const Author& getAuthor() const { return author_; }
    const Publisher& getPublisher() const { return publisher_; }
    const Genre& getGenre() const { return genre_; }
    const Money& getPrice() const { return price_; }
    int getWeightGrams() const { return weightGrams_; }
    int getPages() const { return pages_; }
    int getPublicationYear() const { return publicationYear_; }
};
