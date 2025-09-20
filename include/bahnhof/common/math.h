#pragma once
#include <memory>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <unordered_set>

const double pi = 3.141592653589793238;
const float g = 9.8;
const float normalgauge = 1.435;

struct SDL_FPoint;
struct Angle;

struct Vec
{
    Vec();
    Vec(float xstart, float ystart);
    Vec(const SDL_FPoint& point);
    Vec operator+(const Vec&) const;
    Vec operator-(const Vec&) const;
    Vec operator*(float) const;
    friend Vec operator*(float, const Vec&);
    Vec operator/(float) const;
    float x;
    float y;
};

struct Localvec
{
    Localvec(float xstart, float ystart) : x(xstart), y(ystart) {};
    Localvec(const Vec& origin, const Angle& orientation, const Vec& globalvec);
    float x;
    float y;
};

float norm(const Vec& v);
float normsquared(const Vec& v);
Localvec localcoords(Vec globalvec, Angle angle, Vec origin);
Vec globalcoords(Localvec localvec, Angle angle, Vec origin);

float sign(float a);
float truncate(float dir, float denominator);
float anglediff(float a, float b, float wraparound);
int randint(int maxinclusive);