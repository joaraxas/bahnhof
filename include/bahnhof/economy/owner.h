#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "money.h"
#include "account.h"
#include "stake.h"

class Company;

class Owner
{
public:
    Owner() : Owner(0) {}
    Owner(Money startamount) : account(startamount) {}
    bool buy(Owner& from, Company& company, uint16_t amount);
    bool buy(Stake& fromstake, Account& intoaccount, uint16_t amount);
    Account& getaccount() {return account;}
private:
    Stake* getstakeforcompany(Company& company);
    Account account;
    std::map<Company*, Stake*> stakes;
};