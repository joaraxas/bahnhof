#pragma once
#include<set>
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resourcetypes.h"
#include "bahnhof/buildings/buildingtypes.h"

class Storage;
class Game;
class Shape;
class Rendering;

class Building
{
public:
    Building(Game* game, BuildingID id, std::unique_ptr<Shape> s);
    virtual ~Building();
    virtual void render(Rendering* rendering);
    void update(int ms);
    virtual void trigger() {};
    bool checkclick(Vec pos);
    virtual bool leftclick(Vec pos);
protected:
    SDL_Color color;
    Game* game;
    std::unique_ptr<Shape> shape;
private:
    int timeleft = 3000;
};

class Industry : public Building
{
public:
    Industry(Game* game, BuildingID id, std::unique_ptr<Shape> s, std::set<resourcetype> need, std::set<resourcetype> production);
    void trigger();
private:
    Storage* storage;
    std::set<resourcetype> wants;
    std::set<resourcetype> makes;
};

class WagonFactory : public Building
{
public:
    WagonFactory(Game* game, std::unique_ptr<Shape> s, State st);
    void trigger();
    State& getstate() {return state;};
private:
    State state;
};

class Brewery : public Industry
{
public:
    Brewery(Game* game, std::unique_ptr<Shape> s);
};

class Hopsfield : public Industry
{
public:
    Hopsfield(Game* game, std::unique_ptr<Shape> s);
};

class Barleyfield : public Industry
{
public:
    Barleyfield(Game* game, std::unique_ptr<Shape> s);
};

class City : public Industry
{
public:
    City(Game* game, std::unique_ptr<Shape> s);
};

