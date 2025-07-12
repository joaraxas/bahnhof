#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include <memory>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>

struct Vec
{
    Vec();
    Vec(float xstart, float ystart);
    Vec(const SDL_FPoint& point);
    Vec operator+(Vec);
    Vec operator-(Vec);
    Vec operator*(float);
    Vec operator/(float);
    float x;
    float y;
};

float norm(Vec v);
Vec localcoords(Vec globalvec, float angle, Vec origin);
Vec globalcoords(Vec localvec, float angle, Vec origin);

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

float sign(float a);
float truncate(float dir);
int randint(int maxinclusive);

const double pi = 3.141592653589793238;
const float g = 9.8;
const float normalgauge = 1.435;