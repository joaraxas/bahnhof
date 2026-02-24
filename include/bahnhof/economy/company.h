#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "account.h"
#include "stake.h"
#include "owner.h"

class Game;
class Rendering;
class InterfaceManager;

class Company
{
public:
    Company(std::string name) : 
        owner(name) {};
    const std::string& getname() const {return owner.getname();}
    Account& getaccount() {return owner.getaccount();}
    Money getvalue() const {return valuation;}
    Money getshareprice() const {
        if(shares == 0) return 5;
        return valuation/shares;
    }
    uint16_t getnumshares() const {return shares;}
    void createmainpanel(InterfaceManager* ui);
    bool emission(Money investment, Owner& buyer);
    Stake* const getstakeforowner(Owner& who);
    Stake& registernewstake(Owner& who);
    bool removeemptystake(Owner& who);
    Owner& getcompanysinvestments() {return owner;} // might remove
    const std::map<Owner*,Stake>& getowners() {return stakesincompany;}
private:
    std::string& getnameforedit() {return owner.getnameforedit();}
    Owner owner;
    uint16_t shares{0};
    std::map<Owner*,Stake> stakesincompany;
    Money valuation{0};
    UI::Ownership mainpanel;
};