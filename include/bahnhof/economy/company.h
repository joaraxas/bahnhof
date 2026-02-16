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
    int getvalue() const {return money;}
    bool canafford(int amount) const {return money>=amount;}
    bool pay(int amount, Account* receiver=nullptr){
        if(!canafford(amount)) return false;
        if(amount<=0) return false;
        money-=amount;
        if(receiver)
            receiver->money+=amount;
        return true;
    }
    void earn(int amount){
        if(amount<=0) return;
        // from nowhere
        money+=amount;
    }
private:
    int money;
};

class Company
{
public:
    Company(std::string n) : 
        name(n), account(500), 
        shareprice(account.getvalue()/shares) {};
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