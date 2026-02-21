#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "money.h"

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
    bool pay(Money amount, Account* receiver=nullptr){
        if(!canafford(amount)) return false;
        if(amount<=0) return false;
        money-=amount;
        if(receiver)
            receiver->money+=amount;
        return true;
    }
    void earn(Money amount){
        if(amount<=0) return;
        // from nowhere
        money+=amount;
    }
private:
    Money money;
};