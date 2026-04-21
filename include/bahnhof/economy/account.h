#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "money.h"
#include "payments.h"

class InterfaceManager;

namespace Economy {

class Account
{
public:
    Account(Money cash) : money(cash) {}
    Account(const Account& a) = delete;
    Account& operator=(const Account& a) = delete;
    Money getvalue() const {return money;}
    Money getprofit() const {return profit;}
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
        if(isrecurring(type)){
            profit -= amount;
            receiver->profit += amount;
        }
        return true;
    }
    void earn(Money amount, PaymentType type){
        if(amount<=0) return;
        // from nowhere
        money+=amount;
        income[type] += amount;
        if(isrecurring(type))
            profit += amount;
    }
    void createpanel(InterfaceManager* ui);
private:
    Money money;
    std::map<PaymentType, Money> income;
    std::map<PaymentType, Money> expenses;
    Money profit;
    UI::Ownership panel;
};

} // namespace Economy