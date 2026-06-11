#pragma once
#include "bahnhof/common/forwardincludes.h"

namespace Economy {

class Money
{
public:
    constexpr Money() {Money{0.0};}
    constexpr explicit Money(double v) : amount{v} {}
    friend constexpr Money operator"" _Fr(long double v);
    friend constexpr Money operator"" _Fr(unsigned long long v);

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
    double operator/(const Money rhs) {return amount/rhs.amount;}
private:
    double amount;
};
 
inline constexpr Money operator"" _Fr(long double v){
    return Money{static_cast<double>(v)};
}
inline constexpr Money operator"" _Fr(unsigned long long v){
    return Money{static_cast<double>(v)};
}

constexpr Money operator+(Money lhs, const Money rhs) {return lhs+=rhs;}
constexpr Money operator-(Money lhs, const Money rhs) {return lhs-=rhs;}
constexpr Money operator*(Money lhs, const double rhs) {return lhs*=rhs;}
constexpr Money operator*(const double lhs, Money rhs) {return rhs*lhs;}
constexpr Money operator/(Money lhs, const double div) {return lhs/=div;}

} // namespace Economy

using Economy::operator"" _Fr;