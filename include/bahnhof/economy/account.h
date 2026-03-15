#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "money.h"

namespace Economy {

enum class PaymentType {
    stocks,
    rollingstock,
    buildings,
    ticketfare
};

inline std::string getbudgetpostname(PaymentType transaction)
{
    switch (transaction)
    {
    case PaymentType::stocks:
        return "Stock investments";
    case PaymentType::rollingstock:
        return "Rolling stock";
    case PaymentType::buildings:
        return "Buildings";
    case PaymentType::ticketfare:
        return "Ticket sales";
    default:
        return "Unknown";
    }
}

class Loan
{
private:
    Money amount;
};

class Account
{
public:
    Account(Money cash) : money(cash) {}
    Money getvalue() const {return money;}
    bool canafford(Money amount) const {return money>=amount;}
    bool pay(Money amount, PaymentType type, Account* receiver=nullptr){
        if(!canafford(amount)) return false;
        if(amount<=0) return false;
        money-=amount;
        if(receiver){
            receiver->money += amount;
            receiver->income[type] += amount;
        }
        expenses[type] += amount;
        return true;
    }
    void earn(Money amount, PaymentType type){
        if(amount<=0) return;
        // from nowhere
        money+=amount;
        income[type] += amount;
    }
private:
    Money money;
    std::map<PaymentType, Money> income;
    std::map<PaymentType, Money> expenses;
};

} // namespace Economy