#pragma once
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/spritenames.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/economy/money.h"

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
    Money cost;
    enginetype engine;
    std::unordered_set<resourcetype> storableresources;
    int storagecapacity;
    bool renderstorage = false;
    float forwardpower;
    float backwardpower;
    float forwardmaxspeed;
    float backwardmaxspeed;
};