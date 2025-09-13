#pragma once
#include "math.h"

struct Tangent
{
    Tangent();
    Tangent(float angle);
    double absanglediff(const Tangent&) const;
    Tangent operator -() const {return Tangent(-_angle);};
    Tangent operator +(float rot) const {return Tangent(_angle+rot);};
    friend Tangent operator +(float rot, const Tangent& t) {return t+rot;};
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