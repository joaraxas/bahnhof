#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"

class Storage;
class Route;
class Order;

class Train;
class InputManager;

namespace Tracks{
    class Tracksystem;
}

class Wagon
{
public:
    Wagon(Tracks::Tracksystem* mytracks, State trackstate, sprites::name sprname, sprites::name iconame);
    ~Wagon(); //TODO: should be virtual
    void travel(float pixels);
    virtual void update(int ms);
    virtual void render(Rendering* r);
    virtual State frontendstate();
    virtual State backendstate();
    virtual std::vector<State*> getstates();
    virtual int loadwagon(resourcetype type, int amount);
    virtual int unloadwagon(resourcetype* type);
    virtual float getpower();
    Train* train;
    Vec pos;
    bool alignedforward = true;
    bool hasdriver = false;
    int w;
protected:
    Tracks::Tracksystem* tracksystem = nullptr;
    State state; //should be protected
    Sprite sprite;
    Sprite icon;
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
    float maxspeed[2] = {90,180};
    int imagenumber = 4;
    float imageindex = 0;
    float imagespeed = 2;
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


class Train
{
public:
    Train(Tracks::Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed);
    void getinput(InputManager* input, int ms);
    void update(int ms);
    void checkcollision(int ms, Train* train);
    void render(Rendering* r);
    bool perform(int ms);
    void proceed();
    bool gas(int ms);
    bool brake(int ms);
    bool shiftdirection();
    bool loadall();
    bool unloadall();
    State forwardstate();
    State backwardstate();
    bool split(int where, Route* assignedroute=nullptr);
    void couple(Train& train, bool ismyback, bool ishisback);
    Tracks::Tracksystem* tracksystem;
    float speed;
    bool gasisforward = true;
    std::vector<Wagon*> wagons;
    Route* route = nullptr;
    int orderid = 0;
    bool go = false;
    bool wantstocouple = false;
    bool selected = false;
private:
    bool checkifreachedstate(State goalstate, int ms);
    Sprite light;
    Game* game;
};

