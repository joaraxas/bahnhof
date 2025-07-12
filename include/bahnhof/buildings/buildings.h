#pragma once
#include<set>
#include "bahnhof/resources/resourcetypes.h"
#include "bahnhof/buildings/buildingtypes.h"

class Storage;
class Game;

class Building
{
public:
    Building(Game* game, const BuildingType& type, Vec pos);
    virtual ~Building() {};
    virtual void render(Rendering* rendering);
    void update(int ms);
    virtual void trigger() {};
    bool checkclick(Vec pos);
    virtual bool leftclick(Vec pos) {};
protected:
    SDL_Color color;
    Game* game;
private:
    SDL_Rect rect;
    int timeleft = 3000;
};

class Industry : public Building
{
public:
    Industry(Game* game, const BuildingType& type, Vec pos, std::set<resourcetype> need, std::set<resourcetype> production);
    void trigger();
private:
    Storage* storage;
    std::set<resourcetype> wants;
    std::set<resourcetype> makes;
};

class WagonFactory : public Building
{
public:
    WagonFactory(Game* game, const BuildingType& type, Vec pos);
    WagonFactory(Game* game, const BuildingType& type, nodeid node);
    void trigger();
private:
    State state;
};

class Brewery : public Industry
{
public:
    Brewery(Game* game, const BuildingType& type, Vec pos);
};

class Hopsfield : public Industry
{
public:
    Hopsfield(Game* game, const BuildingType& type, Vec pos);
};

class Barleyfield : public Industry
{
public:
    Barleyfield(Game* game, const BuildingType& type, Vec pos);
};

class City : public Industry
{
public:
    City(Game* game, const BuildingType& type, Vec pos);
};

