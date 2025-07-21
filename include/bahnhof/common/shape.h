#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"

class Rendering;

class Shape
{
public:
    virtual ~Shape() {};
    virtual void renderfilled(Rendering* r, SDL_Color color, bool ported=true, bool zoomed=true) {};
    virtual Vec mid() {return Vec(0,0);};
    virtual bool contains(Vec) {return false;};
};

class Rectangle : public Shape
{
public:
    Rectangle(const SDL_Rect& rect);
    Rectangle(int x, int y, int w, int h);
    Rectangle(Vec pos, int w, int h);
    void renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed);
    Vec mid();
    bool contains(Vec);
protected:
    SDL_Rect rect;
};

class RotatedRectangle : public Shape
{
public:
    RotatedRectangle(float mid_x, float mid_y, int w, int h);
    RotatedRectangle(Vec mid, int w, int h, float rotation);
    RotatedRectangle(float mid_x, float mid_y, int w, int h, float rotation);
    void renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed);
    Vec mid();
    bool contains(Vec);
protected:
    float mid_x;
    float mid_y;
    int w;
    int h;
    float angle;
};
