#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>

// ---------------------------------------------------------------
// Observer Pattern: any class that wants to be notified when a
// reserved book becomes available implements this interface.
// ---------------------------------------------------------------
class Observer {
public:
    virtual ~Observer() = default;
    virtual void onBookAvailable(const std::string &bookId, const std::string &bookTitle) = 0;
    virtual std::string observerId() const = 0;
};

#endif
