#pragma once
#include "math.h"

struct Tangent
{
    Tangent();
    Tangent(float angle);
    float operator -(const Tangent&);
private:
    double _angle;
};


struct Angle
{

};