#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/common/math.h"

namespace UI{

class Coord{
public:
    constexpr Coord() {Coord{0};};
    constexpr Coord(int v) : value{v} {};
    constexpr Coord(float v) : value{static_cast<int>(std::lround(v))} {};
    constexpr Coord(double v) : value{static_cast<int>(std::lround(v))} {};

    operator int() {return value;};

    // Coord& operator+=(int v) {value+=v;};
    // Coord& operator-=(int v) {value-=v;};
    // Coord& operator*=(int v) {value*=v;};
    Coord& operator+=(Coord v) {value+=v.value; return *this;};
    Coord& operator-=(Coord v) {value-=v.value; return *this;};
private:
    int value;
};

// inline Coord operator+(Coord c1, Coord c2) {return c1+=c2;};
// inline Coord operator-(Coord c1, Coord c2) {return c1-=c2;};
// inline Coord operator*(Coord c1, int f) {return c1*=f;};
// inline Coord operator*(int f, Coord c1) {return c1*f;};

struct UIRect{
    UIRect(Coord xx, Coord yy, Coord ww, Coord hh) :
        x{xx}, y{yy}, w{ww}, h{hh} {};
    UIRect(const SDL_Rect& r) : x{r.x}, y{r.y}, w{r.w}, h{r.h} {};

    operator SDL_Rect() {return {x, y, w, h};};

    Coord x;
    Coord y;
    Coord w;
    Coord h;
};

struct UIVec{
    UIVec(Coord xx, Coord yy) : x{xx}, y{yy} {};
    // UIVec(int xx, int yy) : x{xx}, y{yy} {};
    UIVec(Vec v) : x{v.x}, y{v.y} {};
    UIVec& operator+=(UIVec v) {x+=v.x; y+=v.y; return *this;};
    UIVec& operator-=(UIVec v) {x-=v.x; y-=v.y; return *this;};

    operator Vec() {return Vec{float(x),float(y)};};

    Coord x;
    Coord y;
};

inline UIVec operator+(UIVec u, UIVec v) {return u+=v;};
inline UIVec operator-(UIVec u, UIVec v) {return u-=v;};


}