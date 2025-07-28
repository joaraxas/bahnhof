#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/rollingstock/wagontypes.h"

class Game;

class RollingStockManager
{
public:
    RollingStockManager(Game* g);
    const WagonType& gettypefromid(WagonID id) const {return types.at(id);};
private:
    Game* game;
    std::map<WagonID, WagonType> types;
};