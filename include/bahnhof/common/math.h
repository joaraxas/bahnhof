#pragma once
#include "forwardincludes.h"

const double pi = 3.141592653589793238;
const float g = 9.8;

template<typename T>
constexpr double meterstopixels(T m)
{
    return m*1000/150;
}

template<typename T>
constexpr double pixelstometers(T pixels)
{
    return pixels*0.001*150;
}
const float normalgauge = 1.435;
const float ballastwidth = 4.000;
const float sleeperwidth = 2.600;

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
    Angle toangle() const;
    bool iszero() const {return x==0 && y==0;}
    float x;
    float y;
};

struct Localvec
{
    Localvec(float xstart, float ystart) : x(xstart), y(ystart) {};
    Localvec& operator+=(const Localvec& v) {x+=v.x; y+=v.y; return *this;}
    Localvec& operator-=(const Localvec& v) {x-=v.x; y-=v.y; return *this;}
    Localvec operator*(float c) const {return {x*c, y*c};};
    friend Localvec operator*(float c, const Localvec& v) {return v*c;}
    Localvec operator/(float c) const {return {x/c, y/c};};
    Angle toangle() const;
    float x;
    float y;
};

Localvec operator+(Localvec v1, const Localvec& v2);
Localvec operator-(Localvec v1, const Localvec& v2);

std::ostream& operator<<(std::ostream& os, const Vec& v);
std::ostream& operator<<(std::ostream& os, const Localvec& v);

template<typename T> 
float norm(const T& v) {return std::sqrt(normsquared(v));}
template<typename T> 
float normsquared(const T& v) {return v.x*v.x + v.y*v.y;}
Localvec localcoords(const Vec& globalvec, const Angle& angle, const Vec& origin);
Vec globalcoords(const Localvec& localvec, const Angle& angle, const Vec& origin);

float sign(float a);
float truncate(float dir, float denominator);
float anglediff(float a, float b, float wraparound);
int randint(int maxinclusive);
float randfloat(float maxexclusive);