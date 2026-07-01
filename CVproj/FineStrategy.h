#ifndef FINE_STRATEGY_H
#define FINE_STRATEGY_H

#include <string>
#include <algorithm>

// ---------------------------------------------------------------
// Strategy Pattern: the library's fine policy can be swapped at
// runtime without touching any borrowing/returning logic.
// ---------------------------------------------------------------
class FineStrategy {
public:
    virtual ~FineStrategy() = default;
    virtual double calculateFine(int daysLate) const = 0;
    virtual std::string name() const = 0;
};

// Standard flat rate per day late
class StandardFineStrategy : public FineStrategy {
private:
    double ratePerDay;
public:
    explicit StandardFineStrategy(double ratePerDay_ = 5.0) : ratePerDay(ratePerDay_) {}
    double calculateFine(int daysLate) const override {
        return daysLate > 0 ? daysLate * ratePerDay : 0.0;
    }
    std::string name() const override { return "Standard Fine Policy"; }
};

// Discounted rate for student members
class StudentFineStrategy : public FineStrategy {
private:
    double ratePerDay;
    double discount; // e.g. 0.5 = 50% off
public:
    explicit StudentFineStrategy(double ratePerDay_ = 5.0, double discount_ = 0.5)
        : ratePerDay(ratePerDay_), discount(discount_) {}
    double calculateFine(int daysLate) const override {
        return daysLate > 0 ? daysLate * ratePerDay * (1.0 - discount) : 0.0;
    }
    std::string name() const override { return "Student Discounted Fine Policy"; }
};

// Waives the first `graceDays` late days entirely, then charges full rate
class GracePeriodFineStrategy : public FineStrategy {
private:
    double ratePerDay;
    int graceDays;
public:
    explicit GracePeriodFineStrategy(double ratePerDay_ = 5.0, int graceDays_ = 2)
        : ratePerDay(ratePerDay_), graceDays(graceDays_) {}
    double calculateFine(int daysLate) const override {
        int chargeableDays = std::max(0, daysLate - graceDays);
        return chargeableDays * ratePerDay;
    }
    std::string name() const override { return "Grace-Period Fine Policy"; }
};

#endif
