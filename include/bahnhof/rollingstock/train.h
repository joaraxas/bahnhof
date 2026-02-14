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
    ~Train();
    void getinput(InputManager* input, int ms);
    void update(int ms);
    void checkcollision(int ms, Train* train);
    void render(Rendering* r);
    bool perform(int ms);
    void proceed();
    bool cangas();
    void gas(int ms);
    bool brake(int ms);
    bool shiftdirection();
    bool loadall();
    bool unloadall();
    State forwardstate();
    State backwardstate();
    bool split(int where, Route* assignedroute=nullptr);
    void couple(Train& train, bool ismyback, bool ishisback);
    TrainInfo getinfo();
    void select();
    void deselect();
    bool isselected() {return selected;};
    Tracks::Tracksystem* tracksystem;
    float speed;
    bool gasisforward = true;
    std::vector<Wagon*> wagons;
    Route* route = nullptr;
    int orderid = 0;
    bool go = false;
    bool wantstocouple = false;
    std::string name = "no name";
private:
    bool checkifreachedstate(State goalstate, int ms);
    UI::Ownership panel;
    bool selected = false;
    Sprite light;
    Game* game = nullptr;
};