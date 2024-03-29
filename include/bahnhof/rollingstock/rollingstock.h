#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"

class Storage;
class Route;
class Order;

class Tracksystem;
class Train;
class InputManager;

class Wagon
{
public:
    Wagon(Tracksystem* mytracks, State trackstate, sprites::name sprname, sprites::name iconame);
    void travel(float pixels);
    virtual void update(int ms);
    virtual void render(Rendering* r);
    virtual State frontendstate();
    virtual State backendstate();
    virtual int loadwagon(resourcetype type, int amount);
    virtual int unloadwagon(resourcetype* type);
    virtual float getpower();
    Train* train;
    Vec pos;
    bool alignedforward = true;
    bool hasdriver = false;
    int w;
    State state; //should be protected
protected:
    Tracksystem* tracksystem = nullptr;
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
    Locomotive(Tracksystem* mytracks, State trackstate);
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
    Openwagon(Tracksystem* mytracks, State trackstate);
    int loadwagon(resourcetype type, int amount);
};

class Tankwagon : public Wagon
{
public:
    Tankwagon(Tracksystem* mytracks, State trackstate);
    int loadwagon(resourcetype type, int amount);
};


class Train
{
public:
    Train(Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed);
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
    Tracksystem* tracksystem;
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

