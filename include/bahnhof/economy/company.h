#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "playercontrol.h"
#include "account.h"
#include "portfolio.h"
#include "stock.h"
#include "control.h"

class InterfaceManager;
class Building;

namespace Economy{

std::string generateslogan();

class Company : Entity
{
public:
    Company(std::string n, Stockmarket& market, Money startamount=0, 
            bool controlled=false) : 
        playercontrol(controlled),
        name{n}, 
        slogan{generateslogan()},
        account{startamount}, 
        portfolio{*this, account, playercontrol}, 
        stock{*this, account, market},
        buildings{*this, account, playercontrol}
    {}
    const std::string& getname() const override {return name;}
    Portfolio& getcompanysinvestments() {return portfolio;} // might remove
    Stock& getcompanysshares() {return stock;} // might remove
    Account& getcompanysaccount() {return account;} // might remove
    Control<Building>& getcompanysbuildingcontrol() 
        {return buildings;} // might remove
    void createpanel(InterfaceManager* ui);
private:
    PlayerControl playercontrol;
    std::string name;
    std::string slogan;
    Account account;
    Portfolio portfolio;
    Stock stock;
    Control<Building> buildings;
    UI::Ownership panel;
};

} // namespace Economy