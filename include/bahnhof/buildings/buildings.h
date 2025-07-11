#pragma once
#include<set>
#include "bahnhof/resources/resourcetypes.h"

class Storage;
class Game;

class Building
{
public:
    Building(Game* game, int x, int y, int w, int h);
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
    Industry(Game* game, int x, int y, int w, int h, std::set<resourcetype> need, std::set<resourcetype> production);
    void trigger();
private:
    Storage* storage;
    std::set<resourcetype> wants;
    std::set<resourcetype> makes;
};

class WagonFactory : public Building
{
public:
    WagonFactory(Game* game, Vec pos);
    WagonFactory(Game* game, nodeid node);
    void trigger();
private:
    State state;
};

class Brewery : public Industry
{
public:
    Brewery(Game* game, Vec pos);
};

class Hopsfield : public Industry
{
public:
    Hopsfield(Game* game, Vec pos);
};

class Barleyfield : public Industry
{
public:
    Barleyfield(Game* game, Vec pos);
};

class City : public Industry
{
public:
    City(Game* game, Vec pos);
};

