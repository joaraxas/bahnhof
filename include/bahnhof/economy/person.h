#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "money.h"
#include "account.h"
#include "portfolio.h"

class Stock;
class InterfaceManager;

class Person : Entity
{
public:
    Person(std::string n) : Person(n, 0) {}
    Person(std::string n, Money startamount) : 
        name{n}, 
        account{startamount}, 
        portfolio{*this, account} 
    {}
    const std::string& getname() const override {return name;}
    void createpanel(InterfaceManager* ui);
    Portfolio& getinvestments() {return portfolio;} // might remove
    Account& getaccount() {return account;} // might remove
private:
    std::string name;
    Account account;
    Portfolio portfolio;
    UI::Ownership panel;
};