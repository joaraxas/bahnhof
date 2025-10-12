#pragma once
#include "math.h"


struct Angle
{
    Angle() : Angle(0.0) {};
    explicit Angle(float radians) : _angle(truncate(radians, 2*pi)) {};
    explicit Angle(Vec v) : Angle(v.toangle()) {};
    explicit Angle(Localvec v) : Angle(v.toangle()) {};

    Angle operator +(const Angle& a) const {return Angle(_angle+a._angle);};
    Angle& operator +=(const Angle& a) {_angle=truncate(_angle+a._angle, 2*pi); return *this;};
    Angle operator -(const Angle& a) const {return Angle(_angle-a._angle);};
    Angle operator *(float c) const {return Angle(_angle*c);};
    friend Angle operator *(float c, const Angle& a) {return a*c;};
    Angle operator /(int c) const {return Angle(_angle/double(c));};
    double operator /(const Angle& a) const {return _angle/a._angle;};
    Angle operator -() const {return Angle(-_angle);};
    friend bool operator ==(const Angle& lhs, const Angle& rhs) {return lhs._angle == rhs._angle;};
    operator std::string() const {return std::to_string(getdegrees())+"°";};
    friend std::ostream& operator<<(std::ostream& os, const Angle& a) {return os << std::string(a);};

    bool isbetween(const Angle& lower, const Angle& upper);
    double getdegrees() const {return _angle*180/pi;};
    double arclength(double radius) {return radius*_angle;};

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
    explicit Tangent(Vec v) : Tangent(v.toangle()) {};
    explicit Tangent(Localvec v) : Tangent(v.toangle()) {};

    Tangent operator -() const {return Tangent(-_angle);};
    Tangent operator +(float rot) const {return Tangent(_angle+rot);};
    friend Tangent operator +(float rot, const Tangent& t) {return t+rot;};
    Tangent operator +(Angle rot) const {return Tangent(_angle+rot._angle);};
    Tangent operator -(Angle rot) const {return Tangent(_angle-rot._angle);};
    operator std::string() const;
    friend std::ostream& operator<<(std::ostream& os, const Tangent& t) {return os << std::string(t);};

    Angle getradiansup() const {return Angle(_angle);};
    Angle getradiansdown() const {return Angle(_angle+pi);};
    double absanglediff(const Tangent&) const;

    friend double sin(const Tangent&);
    friend double cos(const Tangent&);
    friend double tan(const Tangent&);
private:
    double _angle;
};