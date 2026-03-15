#pragma once
#include "bahnhof/common/forwardincludes.h"

namespace Economy {

class Money
{
public:
    constexpr Money() {Money{0.0};}
    constexpr Money(double v) : amount{v} {}

    constexpr Money& operator+=(const Money other) {amount+=other.amount; return *this;}
    constexpr Money& operator-=(const Money other) {amount-=other.amount; return *this;}
    
    bool operator<(const Money other) const {return amount<other.amount;};
    bool operator>(const Money other) const {return amount>other.amount;};
    bool operator<=(const Money other) const {return amount<=other.amount;};
    bool operator>=(const Money other) const {return amount>=other.amount;};
    bool operator==(const Money other) const {return amount==other.amount;};
    bool operator!=(const Money other) const {return amount!=other.amount;};

    operator std::string() const {return std::format("{0:.2f} Fr", amount);};
    friend std::ostream& operator<<(std::ostream& os, const Money m) {return os << std::string(m);};

    constexpr Money& operator*=(const double rhs) {amount*=rhs; return *this;}
    constexpr Money& operator/=(const double rhs) {amount/=rhs; return *this;}
    friend double operator/(Money lhs, const Money rhs) {return lhs.amount/rhs.amount;}
private:
    double amount;
};

constexpr Money operator+(Money lhs, const Money rhs) {return lhs+=rhs;}
constexpr Money operator-(Money lhs, const Money rhs) {return lhs-=rhs;}
constexpr Money operator*(Money lhs, const double rhs) {return lhs*=rhs;}
constexpr Money operator*(const double lhs, Money rhs) {return rhs*lhs;}
constexpr Money operator/(Money lhs, const double div) {return lhs/=div;}

} // namespace Economy