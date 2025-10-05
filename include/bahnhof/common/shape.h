#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "orientation.h"

class Rendering;
class Rectangle;
class RotatedRectangle;
class AnnularSector;

class Shape
{
public:
    virtual ~Shape() {};
    virtual void renderfilled(Rendering* r, SDL_Color color, bool ported=true, bool zoomed=true) const {};
    virtual Vec mid() const = 0;
    virtual Angle getorientation() const {return Angle::zero;};
    virtual bool contains(Vec) const {return false;};
    virtual bool intersects(const Shape&) const = 0;
    virtual bool intersectsrect(const Rectangle&) const = 0;
    virtual bool intersectsrotrect(const RotatedRectangle&) const = 0;
    virtual bool intersectsannularsector(const AnnularSector&) const = 0;
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
    std::vector<Vec> getvertices() const;
    Vec getsize() const;
    bool intersects(const Shape&) const;
    bool intersectsrect(const Rectangle&) const;
    bool intersectsrotrect(const RotatedRectangle&) const;
    bool intersectsannularsector(const AnnularSector&) const;
protected:
    SDL_Rect rect;
};

class RotatedRectangle : public Shape
{
public:
    RotatedRectangle(float mid_x, float mid_y, int w, int h);
    RotatedRectangle(Vec mid, int w, int h, Angle rotation);
    RotatedRectangle(float mid_x, float mid_y, int w, int h, Angle rotation);
    void renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed) const;
    Vec mid() const;
    Angle getorientation() const {return angle;};
    bool contains(Vec) const;
    std::vector<Vec> getvertices() const;
    Vec getsize() const;
    bool intersects(const Shape&) const;
    bool intersectsrect(const Rectangle&) const;
    bool intersectsrotrect(const RotatedRectangle&) const;
    bool intersectsannularsector(const AnnularSector&) const;
protected:
    float mid_x;
    float mid_y;
    int w;
    int h;
    Angle angle;
};

class AnnularSector : public Shape
{
public:
    AnnularSector(Vec frompos, Angle fromorientation, Angle arcangle, float radius, float thickness);
    void renderfilled(Rendering* r, SDL_Color color, bool ported=true, bool zoomed=true) const;
    Vec mid() const;
    Angle getorientation() const;
    bool contains(Vec) const;
    bool intersects(const Shape&) const;
    bool intersectsrect(const Rectangle&) const;
    bool intersectsrotrect(const RotatedRectangle&) const;
    bool intersectsannularsector(const AnnularSector&) const;
private:
    std::vector<Vec> getvertices() const;
    bool intersectsanyedge(const std::vector<Vec>& orderedvertices) const;
    Vec midpoint;
    float innerradius;
    float outerradius;
    Angle rightlimitangle;
    Angle angle;
    int nSegments;
};

struct Edge
{
    Vec endpoint1;
    Vec endpoint2;
};

struct Arc
{
    Vec center;
    float radius;
    Angle rightangle;
    Angle angle;
};

int discretizecurve(Angle& angle, float radius);

namespace Intersection
{

bool checkprojectionofverticesonrect(const std::vector<Vec>& verts, const std::array<float, 4>& lrtb);
bool checkprojectionofverticesonrotrect(const std::vector<Vec>& verts, const RotatedRectangle& shape);
bool edgesintersect(const Edge&, const Edge&);
bool edgeintersectsarc(const Edge&, const Arc&);
bool anyedgesintersect(const std::vector<Edge>& edges1, const std::vector<Edge>& edges2);

}

