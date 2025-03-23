#pragma once
#include<set>
#include "bahnhof/resources/resourcetypes.h"

class Storage;

class Building
{
public:
    Building(Game* game, int x, int y, int w, int h, std::set<resourcetype> need, std::set<resourcetype> production);
    virtual void render(Rendering* rendering);
    void update(int ms);
protected:
    SDL_Color color;
private:
    Storage* storage;
    SDL_Rect rect;
    int timeleft = 3000;
    Game* game;
    std::set<resourcetype> wants;
    std::set<resourcetype> makes;
};

class Brewery : public Building
{
public:
    Brewery(Game* game, Vec pos);
};

class Hopsfield : public Building
{
public:
    Hopsfield(Game* game, Vec pos);
};

class Barleyfield : public Building
{
public:
    Barleyfield(Game* game, Vec pos);
};

class City : public Building
{
public:
    City(Game* game, Vec pos);
};

