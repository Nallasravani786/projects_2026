#ifndef BOOK_H
#define BOOK_H

#include "Observer.h"
#include <vector>
#include <string>
#include <memory>

// ---------------------------------------------------------------
// Book: abstract base class, and also the "Subject" in the
// Observer pattern -- it keeps a waitlist of Observers (Members)
// who reserved it while unavailable, and notifies them all when
// it's returned.
// ---------------------------------------------------------------
class Book {
protected:
    std::string id;
    std::string title;
    std::string author;
    bool available;
    std::vector<Observer*> waitlist;

public:
    Book(std::string id_, std::string title_, std::string author_)
        : id(std::move(id_)), title(std::move(title_)), author(std::move(author_)), available(true) {}

    virtual ~Book() = default;

    // Each concrete book type defines its own category and loan period
    virtual std::string category() const = 0;
    virtual int loanPeriodDays() const = 0;

    // --- Observer pattern machinery ---
    void addObserver(Observer *o) { waitlist.push_back(o); }

    void notifyObservers() {
        for (Observer *o : waitlist) {
            o->onBookAvailable(id, title);
        }
        waitlist.clear();
    }

    bool hasWaitlist() const { return !waitlist.empty(); }
    size_t waitlistSize() const { return waitlist.size(); }

    // --- Availability ---
    bool isAvailable() const { return available; }

    void setAvailable(bool a) {
        available = a;
        if (a && !waitlist.empty()) {
            notifyObservers();   // book returned -> tell everyone waiting
        }
    }

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
};

// ---------------------------------------------------------------
// Concrete book types -- each has a different loan period,
// reflecting real library policy differences.
// ---------------------------------------------------------------
class FictionBook : public Book {
public:
    FictionBook(std::string id, std::string title, std::string author)
        : Book(std::move(id), std::move(title), std::move(author)) {}
    std::string category() const override { return "Fiction"; }
    int loanPeriodDays() const override { return 14; }
};

class NonFictionBook : public Book {
public:
    NonFictionBook(std::string id, std::string title, std::string author)
        : Book(std::move(id), std::move(title), std::move(author)) {}
    std::string category() const override { return "Non-Fiction"; }
    int loanPeriodDays() const override { return 21; }
};

class MagazineBook : public Book {
public:
    MagazineBook(std::string id, std::string title, std::string author)
        : Book(std::move(id), std::move(title), std::move(author)) {}
    std::string category() const override { return "Magazine"; }
    int loanPeriodDays() const override { return 7; }
};

// ---------------------------------------------------------------
// Factory Pattern: centralizes book creation so calling code never
// needs to know about concrete classes (FictionBook, etc.) -- just
// the type string. Adding a new book category later means adding
// one branch here, with zero changes anywhere else in the system.
// ---------------------------------------------------------------
class BookFactory {
public:
    static std::unique_ptr<Book> createBook(const std::string &type, const std::string &id,
                                              const std::string &title, const std::string &author) {
        if (type == "Fiction")    return std::make_unique<FictionBook>(id, title, author);
        if (type == "NonFiction") return std::make_unique<NonFictionBook>(id, title, author);
        if (type == "Magazine")   return std::make_unique<MagazineBook>(id, title, author);
        return nullptr;
    }
};

#endif
