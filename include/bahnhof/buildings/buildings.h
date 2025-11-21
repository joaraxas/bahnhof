#pragma once
#include<set>
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resourcetypes.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/rollingstock/wagontypes.h"

class Storage;
class Game;
class Shape;
class Rendering;
class RollingStockManager;
namespace UI{
    class Ownership;
}

class Building
{
public:
    Building(Game* g, BuildingID id, std::unique_ptr<Shape> s);
    virtual ~Building();
    virtual void render(Rendering* rendering);
    void update(int ms);
    virtual void trigger() {};
    bool checkcollisionwithpoint(Vec pos);
    bool checkcollisionwithshape(const Shape&);
    virtual bool leftclick(Vec pos);
    std::string name;
    const BuildingType& type;
protected:
    std::unique_ptr<UI::Ownership> panel;
    SDL_Color color;
    Game* game;
    std::unique_ptr<Shape> shape;
    Sprite sprite;
    int timeleft = 3000;
    int timebetweentriggers = 10000;
    bool hassprite = false;
};

class Industry : public Building
{
public:
    Industry(Game* g, BuildingID id, std::unique_ptr<Shape> s, std::set<resourcetype> need, std::set<resourcetype> production);
    void trigger();
private:
    Storage* storage;
    std::set<resourcetype> wants;
    std::set<resourcetype> makes;
};

class WagonFactory : public Building
{
public:
    WagonFactory(Game* g, std::unique_ptr<Shape> s, State st, 
        RollingStockManager& r);
    void trigger();
    bool leftclick(Vec pos);
    State& getstate() {return state;};
    const std::vector<WagonType*> getavailabletypes();
    void orderwagon(const WagonType& type);
    void removefromqueue(int wagonid);
    const std::deque<const WagonType*>& getqueue();
private:
    std::deque<const WagonType*> productionqueue;
    State state;
    RollingStockManager& rollingstock;
    int msleft;
};

class Brewery : public Industry
{
public:
    Brewery(Game* g, std::unique_ptr<Shape> s);
};

class Hopsfield : public Industry
{
public:
    Hopsfield(Game* g, std::unique_ptr<Shape> s);
};

class Barleyfield : public Industry
{
public:
    Barleyfield(Game* g, std::unique_ptr<Shape> s);
};

class City : public Industry
{
public:
    City(Game* g, std::unique_ptr<Shape> s);
};

