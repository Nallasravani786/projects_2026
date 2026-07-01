#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "Member.h"
#include "FineStrategy.h"
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <algorithm>

struct IssueRecord {
    std::string bookId;
    std::string memberId;
    bool active;
};

// ---------------------------------------------------------------
// Singleton Pattern: exactly one Library instance exists for the
// whole application (Meyer's singleton -- thread-safe in C++11+,
// lazily initialized on first use).
// ---------------------------------------------------------------
class Library {
private:
    std::vector<std::unique_ptr<Book>> catalog;
    std::vector<std::unique_ptr<Member>> members;
    std::vector<IssueRecord> issueRecords;
    std::unique_ptr<FineStrategy> fineStrategy;

    Library() : fineStrategy(std::make_unique<StandardFineStrategy>()) {}

public:
    static Library &getInstance() {
        static Library instance;
        return instance;
    }

    Library(const Library &) = delete;
    Library &operator=(const Library &) = delete;

    void setFineStrategy(std::unique_ptr<FineStrategy> strategy) {
        fineStrategy = std::move(strategy);
        std::cout << "[Library] Fine policy switched to: " << fineStrategy->name() << "\n";
    }

    Book *addBook(std::unique_ptr<Book> book) {
        catalog.push_back(std::move(book));
        return catalog.back().get();
    }

    Member *registerMember(const std::string &id, const std::string &name, bool isStudent = false) {
        members.push_back(std::make_unique<Member>(id, name, isStudent));
        return members.back().get();
    }

    Book *findBook(const std::string &bookId) {
        for (auto &b : catalog) if (b->getId() == bookId) return b.get();
        return nullptr;
    }

    Member *findMember(const std::string &memberId) {
        for (auto &m : members) if (m->observerId() == memberId) return m.get();
        return nullptr;
    }

    bool issueBook(const std::string &bookId, const std::string &memberId) {
        Book *b = findBook(bookId);
        Member *m = findMember(memberId);
        if (!b || !m) {
            std::cout << "  Issue failed: book or member not found.\n";
            return false;
        }
        if (!b->isAvailable()) {
            std::cout << "  \"" << b->getTitle() << "\" is currently checked out.\n";
            return false;
        }
        b->setAvailable(false);
        issueRecords.push_back({bookId, memberId, true});
        std::cout << "  Issued \"" << b->getTitle() << "\" to " << m->getName()
                  << " (loan period: " << b->loanPeriodDays() << " days)\n";
        return true;
    }

    void returnBook(const std::string &bookId, const std::string &memberId, int daysLate) {
        Book *b = findBook(bookId);
        if (!b) {
            std::cout << "  Return failed: no such book.\n";
            return;
        }

        for (auto &rec : issueRecords) {
            if (rec.bookId == bookId && rec.memberId == memberId && rec.active) {
                rec.active = false;
                break;
            }
        }

        double fine = fineStrategy->calculateFine(daysLate);
        std::cout << "  \"" << b->getTitle() << "\" returned (" << daysLate << " day(s) late). "
                  << "Fine [" << fineStrategy->name() << "]: Rs." << std::fixed
                  << std::setprecision(2) << fine << "\n";

        b->setAvailable(true);  // triggers Observer notifications if anyone is waiting
    }

    void reserveBook(const std::string &bookId, Member *m) {
        Book *b = findBook(bookId);
        if (!b) {
            std::cout << "  Reservation failed: no such book.\n";
            return;
        }
        if (b->isAvailable()) {
            std::cout << "  \"" << b->getTitle() << "\" is available right now -- no need to reserve.\n";
            return;
        }
        b->addObserver(m);
        std::cout << "  " << m->getName() << " added to waitlist for \"" << b->getTitle()
                  << "\" (position " << b->waitlistSize() << ")\n";
    }

    void printCatalog() {
        std::cout << "\n--- LIBRARY CATALOG ---\n";
        std::cout << std::left << std::setw(6) << "ID" << std::setw(28) << "Title"
                  << std::setw(20) << "Author" << std::setw(14) << "Category"
                  << std::setw(10) << "Status" << "\n";
        for (auto &b : catalog) {
            std::cout << std::left << std::setw(6) << b->getId() << std::setw(28) << b->getTitle()
                      << std::setw(20) << b->getAuthor() << std::setw(14) << b->category()
                      << std::setw(10) << (b->isAvailable() ? "Available" : "Checked Out") << "\n";
        }
    }
};

#endif
