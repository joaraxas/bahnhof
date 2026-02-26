#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "account.h"
#include "portfolio.h"
#include "stock.h"

class InterfaceManager;

class NewCompany : Entity
{
public:
    NewCompany(std::string n) : NewCompany(n, 0) {}
    NewCompany(std::string n, Money startamount) : 
        name{n}, 
        account{startamount}, 
        portfolio{*this, account}, 
        stock{*this, account} 
    {}
    ~NewCompany() {}
    // bool emission(Money investment, NewCompany& buyer);
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
