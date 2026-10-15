#pragma once
#include <map>
#include <string>
#include <vector>
#include "Author.h"
#include "Book.h"
#include "Genre.h"
#include "Publisher.h"

class BookCatalog {
    std::map<std::string, Book> books_;
    std::vector<Genre> genres_;

public:
    void addBook(const Book& book) {
        auto result = books_.emplace(book.getIsbn().getCode(), book);
        if (!result.second) throw DuplicateEntityException("книга " + book.getIsbn().formatted());
    }

    void removeBook(const ISBN& isbn) {
        if (books_.erase(isbn.getCode()) == 0) throw BookNotFoundException(isbn.getCode());
    }

    bool contains(const ISBN& isbn) const { return books_.count(isbn.getCode()) > 0; }

    const Book& findByIsbn(const ISBN& isbn) const {
        auto it = books_.find(isbn.getCode());
        if (it == books_.end()) throw BookNotFoundException(isbn.getCode());
        return it->second;
    }

    std::vector<Book> search(const std::string& query) const {
        std::vector<Book> result;
        for (const auto& entry : books_) {
            if (entry.second.matchesQuery(query)) result.push_back(entry.second);
        }
        return result;
    }

    std::vector<Book> findByAuthor(const Author& author) const {
        std::vector<Book> result;
        for (const auto& entry : books_) {
            if (entry.second.getAuthor().getId() == author.getId()) result.push_back(entry.second);
        }
        return result;
    }

    std::vector<Book> findByGenre(const Genre& genre) const {
        std::vector<Book> result;
        for (const auto& entry : books_) {
            if (entry.second.getGenre().getId() == genre.getId()) result.push_back(entry.second);
        }
        return result;
    }

    std::vector<Book> findByPublisher(const Publisher& publisher) const {
        std::vector<Book> result;
        for (const auto& entry : books_) {
            if (entry.second.getPublisher().getId() == publisher.getId()) result.push_back(entry.second);
        }
        return result;
    }

    void registerGenre(const Genre& genre) {
        for (const auto& g : genres_) {
            if (g.getId() == genre.getId()) throw DuplicateEntityException("жанр " + genre.getName());
        }
        genres_.push_back(genre);
    }

    size_t size() const { return books_.size(); }
    std::vector<Book> allBooks() const {
        std::vector<Book> result;
        for (const auto& entry : books_) result.push_back(entry.second);
        return result;
    }

    const std::vector<Genre>& getGenres() const { return genres_; }
};
