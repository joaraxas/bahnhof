#pragma once
#include<SDL.h>
#include "math.h"

class Camera
{
public:
    Camera();
    Vec mapcoord(Vec sceenpos);
    Vec screencoord(Vec mappos);
    void zoomin(Vec centerpoint);
    void zoomout(Vec centerpoint);
    void pan(Vec direction);
    float getscale();
private:
    void restricttomap();
    SDL_Rect cam;
    float scale = 1;
};