#pragma once
#include<set>
#include "bahnhof/resources/resourcetypes.h"
#include "bahnhof/buildings/buildingtypes.h"

class Storage;
class Game;


class Building
{
public:
    Building(Game* game, BuildingID id, const Shape& s);
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
    Shape shape;
    int timeleft = 3000;
};

class Industry : public Building
{
public:
    Industry(Game* game, BuildingID id, Shape s, std::set<resourcetype> need, std::set<resourcetype> production);
    void trigger();
private:
    Storage* storage;
    std::set<resourcetype> wants;
    std::set<resourcetype> makes;
};

class WagonFactory : public Building
{
public:
    WagonFactory(Game* game, Shape s);
    WagonFactory(Game* game, Shape s, nodeid node);
    void trigger();
private:
    State state;
};

class Brewery : public Industry
{
public:
    Brewery(Game* game, Shape s);
};

class Hopsfield : public Industry
{
public:
    Hopsfield(Game* game, Shape s);
};

class Barleyfield : public Industry
{
public:
    Barleyfield(Game* game, Shape s);
};

class City : public Industry
{
public:
    City(Game* game, Shape s);
};

