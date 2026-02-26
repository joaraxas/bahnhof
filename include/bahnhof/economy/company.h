#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "account.h"
#include "stake.h"
#include "owner.h"

class Game;
class Rendering;
class InterfaceManager;

class Stock
{
public:
    Stock(Entity& e, Account& a) : entity{e}, account{a} {}
    bool emission(Money investment, Portfolio& buyer);
    Money getvalue() const {return valuation;}
    Money getshareprice() const {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    uint16_t getnumshares() const {return shares;}
    Stake* const getstakeforportfolio(Portfolio& who);
    Stake& registernewstake(Portfolio& who);
    bool removeemptystake(Portfolio& who);
    const std::map<Portfolio*,Stake>& getstakes() {return stakes;}
    Entity& getentity() {return entity;}
private:
    Entity& entity;
    Account& account;
    uint16_t shares{0};
    std::map<Portfolio*,Stake> stakes;
    Money valuation{0};
};

class NewCompany : Entity
{
public:
    NewCompany(std::string n) : NewCompany(n, 0) {}
    NewCompany(std::string n, Money startamount) : 
        name{n}, account{startamount}, owner{*this, account}, shares{*this, account} {}
    ~NewCompany() {}
    // bool emission(Money investment, NewCompany& buyer);
    const std::string& getname() const override {return name;}
    Portfolio& getcompanysinvestments() {return owner;} // might remove
    Stock& getcompanysshares() {return shares;} // might remove
    Account& getcompanysaccount() {return account;} // might remove
    void createpanel(InterfaceManager* ui);
private:
    std::string& getnameforedit() {return name;}
    std::string name;
    Account account;
    Portfolio owner;
    Stock shares;
    UI::Ownership panel;
};
