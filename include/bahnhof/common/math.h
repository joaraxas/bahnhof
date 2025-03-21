#pragma once
#include <memory>
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>

class Vec
{
    public:
    Vec();
    Vec(float xstart, float ystart);
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
float truncate(float dir);
int randint(int maxinclusive);

const double pi = 3.141592653589793238;
const float g = 9.8;
const float normalgauge = 1.435;