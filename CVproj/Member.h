#ifndef MEMBER_H
#define MEMBER_H

#include "Observer.h"
#include <iostream>
#include <string>

// ---------------------------------------------------------------
// A library member. Implements Observer so a member can "subscribe"
// to a book's availability by reserving it while it's checked out.
// ---------------------------------------------------------------
class Member : public Observer {
private:
    std::string id;
    std::string name;
    bool isStudent;

public:
    Member(std::string id_, std::string name_, bool isStudent_ = false)
        : id(std::move(id_)), name(std::move(name_)), isStudent(isStudent_) {}

    void onBookAvailable(const std::string &bookId, const std::string &bookTitle) override {
        std::cout << "  [Notification -> " << name << "]: The book \"" << bookTitle
                  << "\" (ID: " << bookId << ") you reserved is now available!\n";
    }

    std::string observerId() const override { return id; }
    std::string getName() const { return name; }
    bool getIsStudent() const { return isStudent; }
};

#endif
