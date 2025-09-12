#pragma once
#include "math.h"

struct Tangent
{
    Tangent();
    Tangent(float angle);
    double operator -(const Tangent&) const;
    double angleup() const {return _angle;};
    double angledown() const {return _angle+pi;};
    operator std::string() const;

    friend double sin(const Tangent&);
    friend double cos(const Tangent&);
    friend double tan(const Tangent&);
private:
    double _angle;
};


struct Angle
{

};