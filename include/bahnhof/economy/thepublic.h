#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/ownership.h"
#include "entity.h"
#include "money.h"
#include "account.h"
#include "portfolio.h"

class InterfaceManager;

namespace Economy {

class Stock;

class ThePublic : Entity
{
public:
    ThePublic() : 
        name{"The public"}, 
        account{1e8f}, 
        portfolio{*this, account} 
    {}
    const std::string& getname() const override {return name;}
    void createpanel(InterfaceManager* ui);
    Portfolio& getinvestments() {return portfolio;} // might remove
private:
    std::string name;
    Account account;
    Portfolio portfolio;
    UI::Ownership panel;
};

} // namespace Economy