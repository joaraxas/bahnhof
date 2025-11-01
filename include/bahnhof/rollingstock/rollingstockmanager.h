#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/rollingstock/wagontypes.h"

class Game;

class RollingStockManager
{
public:
    RollingStockManager(Game* g);
    const WagonType& gettypefromid(WagonID id) const {return types.at(id);};
    const std::vector<WagonType*>& gettypes() {return availabletypes;};
private:
    Game* game;
    std::map<WagonID, WagonType> types;
    std::vector<WagonType*> availabletypes;
};