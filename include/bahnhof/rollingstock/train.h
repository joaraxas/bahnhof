#pragma once
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"


class InputManager;
class Route;
class Train;
class Wagon;
class WagonInfo;

namespace Tracks{
    class Tracksystem;
}

struct WagonInfo
{
    WagonInfo(sprites::name a, resourcetype b, int c) : iconname(a), loadedresource(b), loadamount(c) {};
    sprites::name iconname;
    resourcetype loadedresource;
    int loadamount;
};

struct TrainInfo
{
    TrainInfo(Train* a, std::string b, float c, std::vector<WagonInfo> d) : train(a), name(b), speed(c), wagoninfos(d) {};
    Train* train;
    std::string name;
    float speed;
    std::vector<WagonInfo> wagoninfos;
};

class Train
{
public:
    Train(Tracks::Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons);
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
    TrainInfo getinfo();
    Tracks::Tracksystem* tracksystem;
    float speed;
    bool gasisforward = true;
    std::vector<Wagon*> wagons;
    Route* route = nullptr;
    int orderid = 0;
    bool go = false;
    bool wantstocouple = false;
    bool selected = false;
    std::string name = "no name";
private:
    bool checkifreachedstate(State goalstate, int ms);
    Sprite light;
    Game* game = nullptr;
};

class TrainManager
{
public:
    TrainManager(Tracks::Tracksystem* newtracks);
    void update(int ms);
    void getinput(InputManager* input, int mslogic); 
    void render(Rendering* r);
    void addwagon(Wagon* wagon);
    void addtrain(Train* train);
    void deselectall();
    Train* gettrainatpos(Vec pos);
    std::vector<TrainInfo> gettrainsinfo();
    void inittrain(State startstate);
    void addtrainstoorphans();
private:
    std::string generatetrainname();
    std::vector<std::unique_ptr<Wagon>> wagons;
    std::vector<std::unique_ptr<Train>> trains;
    Tracks::Tracksystem* tracks = nullptr;
};