#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "account.h"
#include "portfolio.h"
#include "stock.h"

class InterfaceManager;

class Company : Entity
{
public:
    Company(std::string n) : Company(n, 0) {}
    Company(std::string n, Money startamount) : 
        name{n}, 
        account{startamount}, 
        portfolio{*this, account}, 
        stock{*this, account} 
    {}
    ~Company() {}
    const std::string& getname() const override {return name;}
    Portfolio& getcompanysinvestments() {return portfolio;} // might remove
    Stock& getcompanysshares() {return stock;} // might remove
    Account& getcompanysaccount() {return account;} // might remove
    void createpanel(InterfaceManager* ui);
private:
    std::string& getnameforedit() {return name;}
    std::string name;
    Account account;
    Portfolio portfolio;
    Stock stock;
    UI::Ownership panel;
};
