#pragma once
#include "bahnhof/common/forwardincludes.h"

namespace UI{

class Coord{
public:
    constexpr Coord() {Coord{0};};
    constexpr Coord(int v) : value{v} {};
    constexpr Coord(double v) : value{static_cast<int>(std::lround(v))} {};

    Coord operator+=(int v) {value+=v; return *this;};
    Coord operator-=(int v) {value-=v; return *this;};
    Coord operator*=(const int v) {value*=v; return *this;};
    Coord operator+=(const Coord v) {value+=v.value; return *this;};
    Coord operator-=(const Coord v) {value-=v.value; return *this;};
    double operator*(const double d) const {return d*value;};
    friend double operator*(const double d, Coord c) {return c*d;};
    Coord operator-() const {return Coord{-value};};

    bool operator<(const Coord c) const {return value<c.value;};
    bool operator>(const Coord c) const {return value>c.value;};
    bool operator<=(const Coord c) const {return value<=c.value;};
    bool operator>=(const Coord c) const {return value>=c.value;};
    bool operator==(const Coord c) const {return value==c.value;};
    bool operator!=(const Coord c) const {return value!=c.value;};

    friend std::ostream& operator<<(std::ostream& o, Coord c) {return o<<c.value;};
private:
    int value;
};

inline Coord operator*(const int i, Coord c) {return c*=i;};
inline Coord operator*(Coord c, const int i) {return c*=i;};

inline Coord operator+(Coord c1, const Coord c2) {return c1+=c2;};
inline Coord operator-(Coord c1, const Coord c2) {return c1-=c2;};
inline Coord operator+(Coord c1, const int i) {return c1+=i;};
inline Coord operator-(Coord c1, const int i) {return c1-=i;};
inline Coord operator+(int i, const Coord c) {return c+i;};
inline Coord operator-(int i, const Coord c) {return c-i;};

struct UIVec{
    UIVec(Coord xx, Coord yy) : x{xx}, y{yy} {};
    UIVec& operator+=(UIVec v) {x+=v.x; y+=v.y; return *this;};
    UIVec& operator-=(UIVec v) {x-=v.x; y-=v.y; return *this;};

    Coord x;
    Coord y;
};

inline UIVec operator+(UIVec u, UIVec v) {return u+=v;};
inline UIVec operator-(UIVec u, UIVec v) {return u-=v;};


struct UIRect{
    UIRect(Coord xx, Coord yy, Coord ww, Coord hh) :
        x{xx}, y{yy}, w{ww}, h{hh} {};
    bool contains(UIVec pos) {
        return pos.x>=x && pos.x<=x+w && pos.y>=y && pos.y<=y+h;
    };

    Coord x;
    Coord y;
    Coord w;
    Coord h;
};

}