#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/sprite.h"

enum BuildingID
{
    brewery,
    hopsfield,
    barleyfield
};

struct BuildingType
{
    BuildingID id;
    std::string name;
    // Vec size;
    // SDL_Color color;
    sprites::name iconname;
    float cost;
};

class BuildingManager
{
public:
    BuildingManager(Game* g);
    const std::vector<BuildingType>& gettypes() {return types;};
private:
    Game* game;
    std::vector<BuildingType> types;
};