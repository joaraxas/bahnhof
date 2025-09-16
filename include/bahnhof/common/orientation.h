#pragma once
#include "math.h"


struct Angle
{
    Angle() : Angle(0.0) {};
    explicit Angle(float radians) : _angle(truncate(radians, 2*pi)) {};

    Angle operator +(const Angle& a) const {return Angle(_angle+a._angle);};
    Angle& operator +=(const Angle& a) {_angle=truncate(_angle+a._angle, 2*pi); return *this;};
    Angle operator -(const Angle& a) const {return Angle(_angle-a._angle);};
    Angle operator *(float c) const {return Angle(_angle*c);}; // TODO: Does this make sense?
    friend Angle operator *(float c, const Angle& a) {return a*c;}; // TODO: Does this make sense?
    Angle operator /(int c) const {return Angle(_angle/double(c));}; // TODO: Does this make sense?
    Angle operator -() const {return Angle(-_angle);};
    friend bool operator ==(const Angle& lhs, const Angle& rhs) {return lhs._angle == rhs._angle;};
    operator std::string() const {std::to_string(_angle);};

    bool isbetween(const Angle& lower, const Angle& upper);
    double getdegrees() const {return _angle*180/pi;};

    static const Angle zero;

    friend double sin(const Angle& a) {return std::sin(a._angle);};
    friend double cos(const Angle& a) {return std::cos(a._angle);};
    friend double tan(const Angle& a) {return std::tan(a._angle);};

    friend struct Tangent;
private:
    double _angle;
};

inline const Angle Angle::zero{0.0};

struct Tangent
{
    Tangent();
    explicit Tangent(float angle);
    explicit Tangent(Angle angle);

    Tangent operator -() const {return Tangent(-_angle);};
    Tangent operator +(float rot) const {return Tangent(_angle+rot);};
    friend Tangent operator +(float rot, const Tangent& t) {return t+rot;};
    operator std::string() const;

    Angle getradiansup() const {return Angle(_angle);};
    Angle getradiansdown() const {return Angle(_angle+pi);};
    double absanglediff(const Tangent&) const;

    friend double sin(const Tangent&);
    friend double cos(const Tangent&);
    friend double tan(const Tangent&);
private:
    double _angle;
};