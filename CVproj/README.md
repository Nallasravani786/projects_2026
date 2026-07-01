# Object-Oriented Library Management System

A C++ library management system built around four classic design patterns
working together, rather than plain CRUD.

## Files
- `Observer.h` — Observer interface
- `Member.h` — Member class implementing Observer
- `Book.h` — Book hierarchy (Subject) + BookFactory (Factory pattern)
- `FineStrategy.h` — three fine policies (Strategy pattern)
- `Library.h` — Singleton orchestrating everything
- `main.cpp` — full demo scenario
- `Project4_OOP_Library_Management_System.pdf` — write-up with verified output

## Compile & Run
```bash
g++ -O2 -std=c++17 main.cpp -o library_system
./library_system
```

## Design Patterns Demonstrated
| Pattern | Where | What it does |
|---|---|---|
| **Factory** | `BookFactory::createBook()` | Creates Fiction/NonFiction/Magazine books without exposing concrete classes |
| **Singleton** | `Library::getInstance()` | Exactly one Library instance, lazily created, thread-safe (Meyer's Singleton) |
| **Observer** | `Book` (Subject) + `Member` (Observer) | Members reserve unavailable books and are auto-notified when returned |
| **Strategy** | `FineStrategy` + 3 implementations | Fine policy (Standard/Student/Grace-Period) swappable at runtime |

## Verified Behavior (from an actual run)
- Two members (Bob, Carol) both reserved the same checked-out book
- When it was returned, **both** were notified automatically, in order
- The same `returnBook()` call produced 3 different fine amounts (₹25, ₹10, ₹5)
  purely by swapping the active Strategy — zero changes to the borrowing logic

## Interview Talking Points
- **"Tell me about a design pattern you've used"** → you have four real,
  working examples instead of one memorized textbook definition
- **Factory vs. direct instantiation** → explain why calling code shouldn't
  know about `FictionBook` vs `MagazineBook` directly
- **Singleton trade-offs** → be ready to discuss why Singletons are sometimes
  criticized (global state, harder to unit test) — a good "what would you
  improve" answer is injecting the Library as a dependency instead
- **Observer vs. polling** → why notification-based reservations are more
  efficient than having every member repeatedly check book availability

## Possible Extensions
- Real database persistence (SQLite/MySQL via a DAO layer)
- Decorator pattern for book variants (audiobook, large-print edition)
- Command pattern to support undo on issue/return actions
