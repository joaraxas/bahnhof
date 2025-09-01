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

float sign(float a);
float truncate(float dir, float denominator=pi);
int randint(int maxinclusive);