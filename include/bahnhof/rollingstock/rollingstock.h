#pragma once
#include<unordered_set>
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"

class Train;
class Wagon;
struct WagonInfo;

namespace Tracks{
    class Tracksystem;
}

namespace RollingStock{
class Axes
{
public:
    Axes(Wagon& w) : wagon(w) {};
    virtual State frontendstate();
    virtual State backendstate();
    virtual std::vector<State*> getstates();
private:
    Wagon& wagon;
};

class Cargo
{
public:
    Cargo(Wagon& w, ResourceManager& r, std::unordered_set<resourcetype> s) : 
        wagon(w), allresources(r), storableresources(s) {};
    virtual int load(const resourcetype type, const int amount);
    virtual int unload(resourcetype& type);
    Resource* getloadedresource();
    resourcetype getloadedresourcetype() {return loadedresource;};
    int getloadedamount() {return loadamount;};
private:
    std::unordered_set<resourcetype> storableresources;
    Wagon& wagon;
    ResourceManager& allresources;
    resourcetype loadedresource = none;
    int loadamount = 0;
    int maxamount = 1;
};

class Engine
{
public:
    Engine(Wagon& w);
    virtual float getpower();
private:
    Wagon& wagon;
    float P[2] = {0.2,0.2};
    float maxspeed[2] = {90,180};
};

} // namespace RollingStock

class Wagon
{
public:
    Wagon(Tracks::Tracksystem* mytracks, State trackstate, sprites::name sprname, sprites::name iconname);
    virtual ~Wagon();
    void travel(float pixels);
    virtual void update(int ms);
    virtual void render(Rendering* r);
    virtual State frontendstate();
    virtual State backendstate();
    virtual std::vector<State*> getstates();
    virtual int loadwagon(resourcetype type, int amount);
    virtual int unloadwagon(resourcetype& type);
    virtual float getpower();
    virtual WagonInfo getinfo();
    Train* train;
    Vec pos;
    bool alignedforward = true;
    bool hasdriver = false;
    int w;
    std::unique_ptr<RollingStock::Axes> axes;
    std::unique_ptr<RollingStock::Cargo> cargo;
    std::unique_ptr<RollingStock::Engine> engine;
protected:
    Tracks::Tracksystem* tracksystem = nullptr;
    State state;
    Sprite sprite;
    Icon icon;
};

class Locomotive : public Wagon
{
public:
    Locomotive(Tracks::Tracksystem* mytracks, State trackstate);
};

class Openwagon : public Wagon
{
public:
    Openwagon(Tracks::Tracksystem* mytracks, State trackstate);
};

class Tankwagon : public Wagon
{
public:
    Tankwagon(Tracks::Tracksystem* mytracks, State trackstate);
};


