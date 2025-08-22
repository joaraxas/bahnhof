#pragma once
#include "bahnhof/common/math.h"
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"

class Train;
class Wagon;
struct WagonInfo;
struct WagonType;

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
    Cargo(Wagon& w, ResourceManager& r, const WagonType& type);
    virtual void render(Rendering* r, Vec pos, float angle, float scale);
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
    bool renderstorage;
};

class Engine
{
public:
    Engine(Wagon& w, const WagonType& type);
    virtual float getpower();
private:
    Wagon& wagon;
    float P[2] = {0,0};
    float maxspeed[2] = {90,180};
};

} // namespace RollingStock

class Wagon
{
public:
    Wagon(Tracks::Tracksystem* mytracks, State trackstate, const WagonType& type);
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