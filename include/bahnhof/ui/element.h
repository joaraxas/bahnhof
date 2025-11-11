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
    virtual UIRect getlocalrect();
    virtual UIVec getminimumsize() {return {rect.w+6, rect.h+4};};
    virtual void place(UIRect r);
protected:
    Host* panel;
    UIRect rect = {0,0,100,100};
    InterfaceManager* ui;
    Game* game;
};

class Layout : public Element
{
public:
    Layout(Host* h) : Element(h) {};
    Layout(Host* h, std::vector<Element*> newels);
    virtual ~Layout() {};
    bool checkclick(UIVec pos) {return false;};
    Element* addelement(Element* el);
    UIVec consolidate();
protected:
    std::vector<Element*> elements;
};

class HBox : public Layout
{
public:
    HBox(Host* h) : Layout(h) {};
    HBox(Host* h, std::vector<Element*> e) : Layout(h, e) {};
    void render(Rendering* r) {};
    virtual void place(UIRect r);
    virtual UIVec getminimumsize();
private:
    std::vector<Coord> minwidths;
};

class VBox : public Layout
{
public:
    VBox(Host* h) : Layout(h) {};
    VBox(Host* h, std::vector<Element*> e) : Layout(h, e) {};
    void render(Rendering* r) {};
    virtual void place(UIRect r);
    virtual UIVec getminimumsize();
private:
    std::vector<Coord> minheights;
};

} // namespace UI