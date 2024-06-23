#pragma once
#include<SDL.h>
#include "math.h"

class Game;

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