#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"
#include "account.h"

class Game;
class Rendering;
class InterfaceManager;

class Company
{
public:
    Company(std::string n) : 
        name(n), account(500), 
        shareprice(account.getvalue()/shares) {};
    std::string& getname() {return name;}
    Account& getaccount() {return account;}
    Money getvalue() {return shares*shareprice;}
    Money getshareprice() {return shareprice;}
    void createmainpanel(InterfaceManager* ui) {
        if(!mainpanel.exists())
    	    mainpanel.set(new UI::CompanyPanel(ui, *this));
    }
private:
    std::vector<Company> daughters;
    std::string name;
    Account account;
    int shares{100};
    Money shareprice;
    UI::Ownership mainpanel;
};