#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "playercontrol.h"
#include "money.h"
#include "account.h"
#include "portfolio.h"

class InterfaceManager;

namespace Economy {

class Stock;
class ControlMode;

class Person : Entity
{
public:
    Person(std::string n, Money startamount=0, bool controlled=false) : 
        playercontrol{controlled},
        name{n}, 
        account{startamount}, 
        portfolio{*this, account, playercontrol} 
    {}
    const std::string& getname() const override {return name;}
    void createpanel(InterfaceManager* ui);
    Portfolio& getinvestments() {return portfolio;} // might remove
    ControlMode generatecontrolmode();
private:
    PlayerControl playercontrol;
    std::string name;
    Account account;
    Portfolio portfolio;
    UI::Ownership panel;
};

} // namespace Economy