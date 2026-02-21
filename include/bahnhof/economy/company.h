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
    const std::string& getname() const {return name;}
    Account& getaccount() {return account;}
    Money getvalue() const {return valuation;}
    Money getshareprice() const {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    void createmainpanel(InterfaceManager* ui) {
        if(!mainpanel.exists())
    	    mainpanel.set(new UI::CompanyPanel(ui, *this, name));
    }
    bool emission(Stake& stake, Money investment, Account& buyer) {
        uint16_t emittedshares = std::lround(
            std::floor(investment/getshareprice()));
        if(emittedshares==0)
            return false;
        Money purchaseamount = emittedshares * getshareprice();
        if(!buyer.canafford(purchaseamount))
            return false;
        if(!addstake(stake))
            return false;
        if(!buyer.pay(purchaseamount, &account))
            return false;
        stake.addamount(emittedshares);
        valuation += emittedshares * getshareprice();
        shares += emittedshares;
        return true;
    }
private:
    bool addstake(Stake& stake) {
        if(!stake.setcompany(*this)) return false;
        stakesincompany.insert(&stake);
        return true;
    }
    std::vector<Company> daughters;
    std::string name;
    Account account{0};
    uint16_t shares{0};
    std::set<Stake*> stakesincompany;
    Money valuation{0};
    UI::Ownership mainpanel;
};