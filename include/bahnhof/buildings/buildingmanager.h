#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/buildings/buildingtypes.h"

class Game;

class BuildingManager
{
public:
    BuildingManager(Game* g);
    const std::vector<BuildingType>& gettypes() {return availabletypes;}; // TODO: would be better to pass an iterator to the map
private:
    Game* game;
    std::map<BuildingID, BuildingType> types;
    std::vector<BuildingType> availabletypes;
};