#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "money.h"
#include "account.h"
#include "stake.h"

class Company;

class Owner
{
public:
    Owner(std::string n) : Owner(n, 0) {}
    Owner(std::string n, Money startamount) : 
        name(n), account(startamount) {}
    bool buy(Owner& from, Company& company, uint16_t amount);
    bool buy(Stake& fromstake, Account& intoaccount, uint16_t amount);
    Account& getaccount() {return account;}
    std::string& getnameforedit() {return name;}
    const std::string& getname() const {return name;}
    const std::set<Company*>& getcompanies() {return companies;}
private:
    Account account;
    std::string name;
    std::set<Company*> companies;
};