#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"
#include "account.h"
#include "stake.h"
#include "owner.h"

class Game;
class Rendering;
class InterfaceManager;

class Company
{
public:
    Company(std::string n) : 
        name(n) {};
    const std::string& getname() const {return name;}
    Account& getaccount() {return owner.getaccount();}
    Money getvalue() const {return valuation;}
    Money getshareprice() const {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    void createmainpanel(InterfaceManager* ui) {
        if(!mainpanel.exists())
    	    mainpanel.set(new UI::CompanyPanel(ui, *this, name));
    }
    bool emission(Money investment, Owner& buyer);
    Stake* registernewstake(Owner& who);
private:
    std::string name;
    Owner owner;
    uint16_t shares{0};
    std::map<Owner*,Stake> stakesincompany;
    Money valuation{0};
    UI::Ownership mainpanel;
};