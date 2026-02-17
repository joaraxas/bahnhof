#pragma once
#include "bahnhof/common/forwardincludes.h"

class Money
{
public:
    constexpr Money() {Money{0};}
    constexpr Money(int v) : amount{std::max(0,v)} {}
    constexpr Money(double v) : amount{std::lround(v)} {}

    Money operator+=(const Money other) {amount+=other.amount; return *this;}
    Money operator-=(const Money other) {amount-=other.amount; if(amount<0) amount=0; return *this;}
    
    bool operator<(const Money other) const {return amount<other.amount;};
    bool operator>(const Money other) const {return amount>other.amount;};
    bool operator<=(const Money other) const {return amount<=other.amount;};
    bool operator>=(const Money other) const {return amount>=other.amount;};
    bool operator==(const Money other) const {return amount==other.amount;};
    bool operator!=(const Money other) const {return amount!=other.amount;};

    operator std::string() const {return std::to_string(amount)+" Fr";};
    friend std::ostream& operator<<(std::ostream& os, const Money m) {return os << std::string(m);};

    friend float operator/(Money lhs, const float rhs) {return lhs.amount/rhs;}
private:
    long amount;
};

inline Money operator+(Money lhs, const Money rhs) {return lhs+=rhs;}
inline Money operator-(Money lhs, const Money rhs) {return lhs-=rhs;}
