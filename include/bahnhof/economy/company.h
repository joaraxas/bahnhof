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
class Storage;

namespace Economy{

class ControlMode;

std::string generateslogan();

class Company : Entity
{
public:
    Company(std::string n, Stockmarket& market, 
            std::vector<std::pair<Portfolio*,Money>> owners={}, 
            bool controlled=false);
    const std::string& getname() const override {return name;}
    Portfolio& getinvestments() {return portfolio;} // might remove
    Control<Storage>& getstorages() {return storages;} // should remove
    void createpanel(InterfaceManager* ui);
    ControlMode generatecontrolmode();
private:
    PlayerControl playercontrol;
    std::string name;
    std::string slogan;
    Account account;
    Portfolio portfolio;
    Stock stock;
    Control<Building> buildings;
    Control<Storage> storages;
    UI::Ownership panel;
};

} // namespace Economy