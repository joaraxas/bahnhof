#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "money.h"
#include "payments.h"

class InterfaceManager;

namespace Economy {

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
    void createpanel(InterfaceManager* ui);
private:
    Money money;
    std::map<PaymentType, Money> income;
    std::map<PaymentType, Money> expenses;
    UI::Ownership panel;
};

} // namespace Economy