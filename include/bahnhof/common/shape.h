#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"

class Rendering;
class Rectangle;
class RotatedRectangle;

class Shape
{
public:
    virtual ~Shape() {};
    virtual void renderfilled(Rendering* r, SDL_Color color, bool ported=true, bool zoomed=true) const {};
    virtual Vec mid() const {return Vec(0,0);};
    virtual bool contains(Vec) const {return false;};
    virtual bool intersects(const Shape*) const = 0;
    virtual bool intersectsrect(const Rectangle*) const = 0;
    virtual bool intersectsrotrect(const RotatedRectangle*) const = 0;
    virtual float getorientation() const {return 0;};
};

class Rectangle : public Shape
{
public:
    Rectangle(const SDL_Rect& rect);
    Rectangle(int x, int y, int w, int h);
    Rectangle(Vec pos, int w, int h);
    Rectangle(Vec pos, Vec size);
    void renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed) const;
    Vec mid() const;
    bool contains(Vec) const;
    bool intersects(const Shape*) const;
    bool intersectsrect(const Rectangle*) const;
    bool intersectsrotrect(const RotatedRectangle*) const;
protected:
    SDL_Rect rect;
};

class RotatedRectangle : public Shape
{
public:
    RotatedRectangle(float mid_x, float mid_y, int w, int h);
    RotatedRectangle(Vec mid, int w, int h, float rotation);
    RotatedRectangle(float mid_x, float mid_y, int w, int h, float rotation);
    void renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed) const;
    Vec mid() const;
    float getorientation() const {return angle;};
    bool contains(Vec) const;
    std::array<Vec, 4> getvertices() const;
    bool intersects(const Shape*) const;
    bool intersectsrect(const Rectangle*) const;
    bool intersectsrotrect(const RotatedRectangle*) const;
protected:
    float mid_x;
    float mid_y;
    int w;
    int h;
    float angle;
};

namespace Intersection
{
bool checkprojectionofverticesonrect(std::array<Vec, 4> verts, std::array<float, 4> rltb);
bool rectangleandrectangle();
bool rectangleandrotrectangle();
bool rotrectangleandrotrectangle();
}