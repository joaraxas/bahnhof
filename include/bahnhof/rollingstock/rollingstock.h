#pragma once
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"

class Train;
struct WagonInfo;

namespace Tracks{
    class Tracksystem;
}

namespace RollingStock{
class Axes
{
public:
    virtual State frontendstate();
    virtual State backendstate();
    virtual std::vector<State*> getstates();
private:
    bool alignedforward = true;
};

class Cargo
{
public:
    virtual int load(resourcetype type, int amount);
    virtual int unload(resourcetype* type);
    resourcetype getloadedresource();
private:
    ResourceManager& allresources;
    resourcetype loadedresource = none;
    int loadamount = 0;
    int maxamount = 1;
};

class Engine
{
public:
    virtual float getpower();
    bool hasdriver = true;
private:
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
    virtual int unloadwagon(resourcetype* type);
    virtual float getpower();
    virtual WagonInfo getinfo();
    Train* train;
    Vec pos;
    bool alignedforward = true;
    bool hasdriver = false;
    int w;
protected:
    Tracks::Tracksystem* tracksystem = nullptr;
    State state;
    Sprite sprite;
    Icon icon;
    ResourceManager* allresources = nullptr;
private:
    resourcetype loadedresource = none;
    int loadamount = 0;
    int maxamount = 1;
};

class Locomotive : public Wagon
{
public:
    Locomotive(Tracks::Tracksystem* mytracks, State trackstate);
    void update(int ms);
    int loadwagon(resourcetype type, int amount);
    int unloadwagon(resourcetype* type);
    float getpower();
private:
    float P[2] = {0.2,0.2};
    float maxspeed[2] = {90,180}; //backwards, forwards
};

class Openwagon : public Wagon
{
public:
    Openwagon(Tracks::Tracksystem* mytracks, State trackstate);
    int loadwagon(resourcetype type, int amount);
};

class Tankwagon : public Wagon
{
public:
    Tankwagon(Tracks::Tracksystem* mytracks, State trackstate);
    int loadwagon(resourcetype type, int amount);
};


