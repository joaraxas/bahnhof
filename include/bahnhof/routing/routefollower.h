#pragma once
#include "bahnhof/common/forwardincludes.h"

class Route;
class Train;
namespace Tracks{
    struct Tracksystem;
}

class RouteFollower
{
    using Tracksystem = Tracks::Tracksystem;
public:
    RouteFollower(Train& t, Tracksystem& tr) : 
        train{t}, tracksystem{&tr} {}
    void trigger(int ms) {if(go) perform(ms);}
    void setorderid(int orderid);
    int getorderid();
    Route* route = nullptr;
    bool go = false;
private:
    bool perform(int ms);
    void proceed();
    Train& train;
    Tracksystem* tracksystem;
    int orderid = 0;
};
