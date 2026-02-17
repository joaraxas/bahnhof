#pragma once
#include "bahnhof/common/forwardincludes.h"


class Loan
{
private:
    int amount;
};

class Account
{
public:
    Account(int cash) : money(cash) {};
    int getvalue() const {return money;}
    bool canafford(int amount) const {return money>=amount;}
    bool pay(int amount, Account* receiver=nullptr){
        if(!canafford(amount)) return false;
        if(amount<=0) return false;
        money-=amount;
        if(receiver)
            receiver->money+=amount;
        return true;
    }
    void earn(int amount){
        if(amount<=0) return;
        // from nowhere
        money+=amount;
    }
private:
    int money;
};
