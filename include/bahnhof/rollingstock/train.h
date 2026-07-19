#pragma once
#include "bahnhof/ui/ownership.h"
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/routing/routefollower.h"


class InputManager;
class Route;
class Wagon;
struct TrainInfo;
namespace Tracks{
    struct Tracksystem;
}

class Train
{
public:
    Train(Tracks::Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons);
    void update(int ms);
    void checkcollision(int ms, Train* train);
    void render(Rendering* r);
    bool cangas();
    void gas(int ms);
    bool brake(int ms);
    bool shiftdirection();
    bool loadall();
    bool unloadall();
    State forwardstate();
    State backwardstate();
    bool split(int where, Route* assignedroute=nullptr);
    TrainInfo getinfo();
    void createpanel();
    bool hasnowagons() {return wagons.empty();}
    float speed;
    bool gasisforward = true;
    bool wantstocouple = false;
    std::string name = "no name";
    bool checkifreachedstate(State goalstate, int ms);
private:
    void couple(Train& train, bool ismyback, bool ishisback);
    std::vector<Wagon*> wagons;
    Tracks::Tracksystem* tracksystem;
    UI::Ownership panel;
    Sprite light;
    RouteFollower routefollower;
    Game* game = nullptr;
};