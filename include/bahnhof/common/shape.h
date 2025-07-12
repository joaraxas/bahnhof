#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"

class Rendering;

class Shape
{
public:
    Shape(float x, float y, int w, int h);
    Shape(float x, float y, int w, int h, float topleftrotation);
    Shape(SDL_Rect& rect);
    Shape(SDL_Rect& rect, float topleftrotation);
    void renderfilled(Rendering* r, SDL_Color color, bool ported=true, bool zoomed=true);
    Vec mid();
protected:
    float x;
    float y;
    int w;
    int h;
    float angle;
};
