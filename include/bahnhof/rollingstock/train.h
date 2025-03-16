#pragma once
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"


class Route;
struct TrainInfo;

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