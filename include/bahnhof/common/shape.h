#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "orientation.h"

class Rendering;
class Rectangle;
class RotatedRectangle;
class Ringsector;

class Shape
{
public:
    virtual ~Shape() = default;
    virtual void renderfilled(Rendering* r, SDL_Color color, bool ported=true, bool zoomed=true) const = 0;
    virtual Vec mid() const = 0;
    virtual Angle getorientation() const {return Angle::zero;};
    virtual bool contains(Vec) const = 0;
    virtual bool intersects(const Shape&) const = 0;
    virtual bool intersectsrect(const Rectangle&) const = 0;
    virtual bool intersectsrotrect(const RotatedRectangle&) const = 0;
    virtual bool intersectsringsector(const Ringsector&) const = 0;
};
