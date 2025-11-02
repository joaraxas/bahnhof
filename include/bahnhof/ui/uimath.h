#pragma once
#include "bahnhof/common/forwardincludes.h"

namespace UI{

class UICoord{
public:
    UICoord() {UICoord{0};};
    UICoord(int v) : value{v} {};
private:
    int value;
};

struct UIRect{
UIRect(UICoord xx, UICoord yy, UICoord ww, UICoord hh) :
    x{xx}, y{yy}, w{ww}, h{hh} {};
UIRect(const SDL_Rect& r) : x{r.x}, y{r.y}, w{r.w}, h{r.h} {};
UICoord x;
UICoord y;
UICoord w;
UICoord h;
};

struct UIVec{
    UICoord x;
    UICoord y;
};


}