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
    Axes(Wagon& w, Tracks::Tracksystem& t, State midstate);
    ~Axes();
    virtual Vec getpos();
    virtual float getorientation();
    virtual void travel(float pixels);
    virtual State frontendstate();
    virtual State backendstate();
    virtual std::vector<State*> getstates();
private:
    Wagon& wagon;
    Tracks::Tracksystem& tracks;
    State state;
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
    const std::unordered_set<resourcetype> storableresources;
    Wagon& wagon;
    ResourceManager& allresources;
    resourcetype loadedresource = none;
    int loadamount = 0;
    const int maxamount = 1;
};

class Engine
{
public:
    Engine(Wagon& w);
    virtual float getpower();
private:
    Wagon& wagon;
    const float P[2] = {0.2,0.2};
    const float maxspeed[2] = {90,180};
};

} // namespace RollingStock

class Wagon
{
public:
    Wagon(Tracks::Tracksystem* mytracks, State trackstate, sprites::name sprname, sprites::name iconname);
    virtual ~Wagon();
    virtual void update(int ms);
    virtual void render(Rendering* r);
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
protected:
    std::unique_ptr<RollingStock::Cargo> cargo;
    std::unique_ptr<RollingStock::Engine> engine;
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


