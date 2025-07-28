#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/spritenames.h"
#include "bahnhof/resources/resources.h"

enum class WagonID
{
    tanklocomotive,
    openwagon,
    refrigeratorcar
};

enum class enginetype
{
    none,
    steam,
    diesel,
    electric
};

struct WagonType
{
    WagonID id;
    std::string name;
    sprites::name iconname;
    sprites::name spritename;
    enginetype engine;
    std::unordered_set<resourcetype> storableresources;
    int storagecapacity;
    float forwardpower;
    float backwardpower;
    float forwardmaxspeed;
    float backwardmaxspeed;
};