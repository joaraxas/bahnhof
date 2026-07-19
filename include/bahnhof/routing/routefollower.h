#pragma once
#include "bahnhof/common/forwardincludes.h"

class Route;
class Train;
namespace Tracks{
    struct Tracksystem;
}

class RouteFollower
{
public:
    void trigger(int ms) {if(go) perform(ms);}
    Train& train;
    Tracks::Tracksystem* tracksystem;
    Route* route = nullptr;
    int orderid = 0;
    bool go = false;
private:
    bool perform(int ms);
    void proceed();
};
