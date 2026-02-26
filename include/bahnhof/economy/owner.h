#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "money.h"
#include "account.h"
#include "stake.h"
#include "entity.h"

class Stock;
class InterfaceManager;

class Portfolio
{
public:
    Portfolio(Entity& e, Account& a) : entity{e}, account{a} {}
    bool buy(Portfolio& fromportfolio, Stock& stock, uint16_t amount);
    bool buy(Stake& fromstake, Account& payableaccount, uint16_t amount);
    Account& getaccount() {return account;}
    const std::set<Stock*>& getstocks() {return stocks;}
    Entity& getentity() {return entity;}
private:
    Entity& entity;
    Account& account;
    std::set<Stock*> stocks;
};

class Person : Entity
{
public:
    Person(std::string n) : Person(n, 0) {}
    Person(std::string n, Money startamount) : 
        name{n}, account{startamount}, portfolio{*this, account} {}
    const std::string& getname() const override {return name;}
    void createpanel(InterfaceManager* ui);
    Portfolio& getinvestments() {return portfolio;} // might remove
private:
    std::string name;
    Account account;
    Portfolio portfolio;
    UI::Ownership panel;
};