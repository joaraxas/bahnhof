#pragma once
#include "bahnhof/common/forwardincludes.h"

class Money
{
public:
    constexpr Money() {Money{0.0};}
    constexpr Money(double v) : amount{v} {}

    Money operator+=(const Money other) {amount+=other.amount; return *this;}
    Money operator-=(const Money other) {amount-=other.amount; return *this;}
    
    bool operator<(const Money other) const {return amount<other.amount;};
    bool operator>(const Money other) const {return amount>other.amount;};
    bool operator<=(const Money other) const {return amount<=other.amount;};
    bool operator>=(const Money other) const {return amount>=other.amount;};
    bool operator==(const Money other) const {return amount==other.amount;};
    bool operator!=(const Money other) const {return amount!=other.amount;};

    operator std::string() const {return std::format("{0:.2f} Fr", amount);};
    friend std::ostream& operator<<(std::ostream& os, const Money m) {return os << std::string(m);};

    friend float operator/(Money lhs, const float rhs) {return lhs.amount/rhs;}
private:
    double amount;
};

inline Money operator+(Money lhs, const Money rhs) {return lhs+=rhs;}
inline Money operator-(Money lhs, const Money rhs) {return lhs-=rhs;}
