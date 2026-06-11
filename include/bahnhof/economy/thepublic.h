#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "playercontrol.h"
#include "money.h"
#include "account.h"
#include "portfolio.h"
#include "control.h"

class InterfaceManager;
class Building;

namespace Economy {

class Stock;

class ThePublic : public  Entity
{
public:
    ThePublic() : 
        account{1.0e8_Fr}, 
        portfolio{*this, account, playercontrol},
        buildings{*this, account, playercontrol}
    {}
    const std::string& getname() const override {return name;}
    void createpanel(InterfaceManager* ui) override;
    Portfolio& getinvestments() {return portfolio;} // might remove
    Control<Building>& getbuildings() {return buildings;} // might remove
private:
    PlayerControl playercontrol{false};
    std::string name{"The public"};
    Account account;
    Portfolio portfolio;
    Control<Building> buildings;
    UI::Ownership panel;
};

} // namespace Economy