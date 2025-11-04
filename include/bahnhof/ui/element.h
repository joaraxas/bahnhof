#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/uimath.h"

class Game;
class Rendering;
class InterfaceManager;

namespace UI{

class Host;

class Element
{
public:
    Element(Host*);
    virtual ~Element() {};
    virtual bool checkclick(UIVec pos);
    virtual void mousehover(UIVec pos, int ms) {};
    virtual void leftclick(UIVec pos) {};
    virtual void scroll(UIVec pos, int distance) {};
    virtual void leftpressed(UIVec pos, int mslogic) {};
    virtual void update(int ms) {};
    virtual void render(Rendering*) = 0;
    virtual UIRect getglobalrect();
    UIRect getlocalrect();
protected:
    Host* panel;
    UIRect rect = {0,0,100,100};
    InterfaceManager* ui;
    Game* game;
};

} // namespace UI