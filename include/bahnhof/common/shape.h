#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"

class Rendering;

class Shape
{
public:
    virtual void renderfilled(Rendering* r, SDL_Color color, bool ported=true, bool zoomed=true) {};
    virtual Vec mid() {return Vec(0,0);};
};

class RotatedRectangle : public Shape
{
public:
    RotatedRectangle(float x, float y, int w, int h);
    RotatedRectangle(float x, float y, int w, int h, float topleftrotation);
    RotatedRectangle(SDL_Rect& rect);
    RotatedRectangle(SDL_Rect& rect, float topleftrotation);
    void renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed);
    Vec mid();
protected:
    float x;
    float y;
    int w;
    int h;
    float angle;
};
