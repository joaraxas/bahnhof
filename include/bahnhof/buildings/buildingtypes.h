#pragma once
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/spritenames.h"

enum BuildingID
{
    brewery,
    hopsfield,
    barleyfield,
    city,
    wagonfactory
};

struct BuildingType
{
    BuildingID id;
    std::string name;
    Vec size;
    SDL_Color color;
    sprites::name spritename;
    sprites::name iconname;
    float cost;
};