#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "money.h"
#include "account.h"
#include "stake.h"
#include "entity.h"

class Company;
class InterfaceManager;

class Owner
{
public:
    Owner(Entity& e, Account& a) : entity{e}, account{a} {}
    bool buy(Owner& from, Company& company, uint16_t amount);
    bool buy(Stake& fromstake, Account& intoaccount, uint16_t amount);
    Account& getaccount() {return account;}
    const std::set<Company*>& getcompanies() {return companies;}
    Entity& getentity() {return entity;}
private:
    Entity& entity;
    Account& account;
    std::set<Company*> companies;
};

class Person : Entity
{
public:
    Person(std::string n) : Person(n, 0) {}
    Person(std::string n, Money startamount) : 
        name{n}, account{startamount}, owner{*this, account} {}
    const std::string& getname() const override {return name;}
    void createpanel(InterfaceManager* ui);
    Owner& getinvestments() {return owner;} // might remove
private:
    std::string name;
    Account account;
    Owner owner;
    UI::Ownership panel;
};