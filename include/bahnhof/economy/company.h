#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"
#include "account.h"
#include "stake.h"

class Game;
class Rendering;
class InterfaceManager;

class Company
{
public:
    Company(std::string n) : 
        name(n) {};
    std::string& getname() {return name;}
    Account& getaccount() {return account;}
    Money getvalue() {return valuation;}
    Money getshareprice() {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    void createmainpanel(InterfaceManager* ui) {
        if(!mainpanel.exists())
    	    mainpanel.set(new UI::CompanyPanel(ui, *this));
    }
    bool addstake(Stake& stake) {
        if(!stake.setcompany(*this)) return false;
        stakes.insert(&stake);
        return true;
    }
    bool emission(Stake& stake) {
        if(!addstake(stake))
            return false;
        Money investment = stake.getamount() * getshareprice();
        shares += stake.getamount();
        valuation += investment;
        account.earn(investment);
        return true;
    }
private:
    std::vector<Company> daughters;
    std::string name;
    Account account{0};
    uint16_t shares{0};
    std::set<Stake*> stakes;
    Money valuation{0};
    UI::Ownership mainpanel;
};