#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"

#include "bahnhof/ui/panels.h"

class Game;
class Rendering;
class InterfaceManager;

class Loan
{
private:
    int amount;
};

class Account
{
public:
    Account(int cash) : money(cash) {};
    int getvalue() {return money;}
    bool canafford(int amount) {return money>=amount;}
    bool pay(int amount) {money-=amount;}
    void earn(int amount) {money+=amount;}
private:
    int money;
};

class Company
{
public:
    Company() : account(600), shareprice(account.getvalue()/shares) {};
    std::string& getname() {return name;}
    Account& getaccount() {return account;}
    float getvalue() {return shares*shareprice;}
    float getshareprice() {return shareprice;}
    void createmainpanel(InterfaceManager* ui) {
        if(!mainpanel.exists())
    	    mainpanel.set(new UI::CompanyPanel(ui, *this));
    }
private:
    std::vector<Company> daughters;
    std::string name;
    Account account;
    int shares{100};
    float shareprice;
    UI::Ownership mainpanel;
};