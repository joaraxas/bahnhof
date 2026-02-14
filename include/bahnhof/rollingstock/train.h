#pragma once
#include "bahnhof/ui/ownership.h"
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"


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
    void getinput(InputManager* input, int ms);
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
    void select();
    void deselect();
    bool isselected() {return selected;};
    bool hasnowagons() {return wagons.empty();}
    float speed;
    bool gasisforward = true;
    Route* route = nullptr;
    int orderid = 0;
    bool go = false;
    bool wantstocouple = false;
    std::string name = "no name";
private:
    bool perform(int ms);
    void proceed();
    bool checkifreachedstate(State goalstate, int ms);
    void couple(Train& train, bool ismyback, bool ishisback);
    std::vector<Wagon*> wagons;
    Tracks::Tracksystem* tracksystem;
    UI::Ownership panel;
    bool selected = false;
    Sprite light;
    Game* game = nullptr;
};