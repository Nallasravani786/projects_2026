// Object-Oriented Library Management System
// ---------------------------------------------
// Demonstrates four classic design patterns working together:
//   - Factory Pattern:   BookFactory creates Fiction/NonFiction/Magazine books
//   - Singleton Pattern: Library::getInstance() -- exactly one library instance
//   - Observer Pattern:  Members reserve unavailable books and get notified
//                        automatically when they're returned
//   - Strategy Pattern:  Fine calculation policy is swappable at runtime
//
// Compile: g++ -O2 -std=c++17 main.cpp -o library_system
// Run:     ./library_system

#include "Library.h"
#include "FineStrategy.h"
#include <iostream>

int main() {
    std::cout << "=== Object-Oriented Library Management System ===\n";
    std::cout << "(Design Patterns: Factory, Singleton, Observer, Strategy)\n";

    // --- Singleton: get the one Library instance ---
    Library &library = Library::getInstance();

    // --- Factory Pattern: create books without knowing concrete classes ---
    std::cout << "\n[1] Adding books via BookFactory...\n";
    library.addBook(BookFactory::createBook("Fiction", "B001", "1984", "George Orwell"));
    library.addBook(BookFactory::createBook("NonFiction", "B002", "Sapiens", "Yuval Noah Harari"));
    library.addBook(BookFactory::createBook("Magazine", "B003", "National Geographic - July", "Various"));
    library.printCatalog();

    // --- Register members ---
    std::cout << "\n[2] Registering members...\n";
    Member *alice = library.registerMember("M01", "Alice", false);
    Member *bob   = library.registerMember("M02", "Bob", true);   // student
    Member *carol = library.registerMember("M03", "Carol", true); // student
    std::cout << "  Registered: Alice, Bob (student), Carol (student)\n";

    // --- Issue "1984" to Alice ---
    std::cout << "\n[3] Alice borrows \"1984\"...\n";
    library.issueBook("B001", "M01");

    // --- Bob and Carol try to borrow the same book -> unavailable -> reserve ---
    std::cout << "\n[4] Bob tries to borrow \"1984\" (unavailable, so he reserves it)...\n";
    library.issueBook("B001", "M02");
    library.reserveBook("B001", bob);

    std::cout << "\n[5] Carol also tries, and reserves it too (waitlist grows)...\n";
    library.issueBook("B001", "M03");
    library.reserveBook("B001", carol);

    // --- Alice returns the book late -> triggers Observer notifications ---
    std::cout << "\n[6] Alice returns \"1984\" 5 days late (Standard Fine Policy in effect)...\n";
    library.returnBook("B001", "M01", 5);

    // --- Demonstrate Strategy pattern: swap fine policy at runtime ---
    std::cout << "\n[7] Switching to Student Discounted Fine Policy for Bob...\n";
    library.setFineStrategy(std::make_unique<StudentFineStrategy>(5.0, 0.5));
    library.issueBook("B002", "M02");             // Bob borrows Sapiens
    library.returnBook("B002", "M02", 4);          // returns it 4 days late

    std::cout << "\n[8] Switching to Grace-Period Fine Policy (first 2 late days free)...\n";
    library.setFineStrategy(std::make_unique<GracePeriodFineStrategy>(5.0, 2));
    library.issueBook("B003", "M03");              // Carol borrows the magazine
    library.returnBook("B003", "M03", 3);           // 3 days late -> only 1 chargeable day

    std::cout << "\n[9] Final catalog state:\n";
    library.printCatalog();

    return 0;
}
