#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/ui/panels.h"
#include "account.h"

class Game;
class Rendering;
class InterfaceManager;

class Stake
{
public:
    Stake(int howmany) : amount(howmany) {};
    bool buyfrom(Stake& from, int howmany) {
        if(from.company!=company) return false;
        if(from.amount>=howmany){
            from.amount -= howmany;
            amount += howmany;
            return true;
        }
        return false;
    }
    int getamount() {return amount;}
    bool setcompany(Company& c) {
        if(!company){
            company = &c;
            return true;
        }
        return false;
    }
private:
    int amount;
    Company* company;
};

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
    void addstake(Stake& stake) {
        stakes.push_back(&stake);
        stake.setcompany(*this);
    }
    void emission(Stake& stake) {
        addstake(stake);
        Money investment = stake.getamount() * getshareprice();
        shares += stake.getamount();
        valuation += investment;
        account.earn(investment);
    }
private:
    std::vector<Company> daughters;
    std::string name;
    Account account{0};
    uint16_t shares{0};
    std::vector<Stake*> stakes;
    Money valuation{0};
    UI::Ownership mainpanel;
};