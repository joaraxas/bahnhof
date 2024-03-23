#pragma once
#include<SDL.h>
#include "bahnhof/common/gamestate.h"
#include "math.h"

class Camera
{
public:
    Camera(Game* whatgame);
    Vec mapcoord(Vec sceenpos);
    Vec screencoord(Vec mappos);
    void zoomin(Vec centerpoint);
    void zoomout(Vec centerpoint);
    void pan(Vec direction);
    float getscale();
private:
    Game* game;
    void restricttomap();
    SDL_Rect cam;
    int logscale = 0;
};