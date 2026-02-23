#pragma once
#include "bahnhof/common/forwardincludes.h"
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
    std::string& getname() {return name;}
private:
    Stake* getstakeforcompany(Company& company);
    Account account;
    std::string name;
    std::map<Company*, Stake*> stakes;
};