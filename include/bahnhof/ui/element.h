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
    virtual UIVec getminimumsize() {return {rect.w+2*mindist.x, rect.h+2*mindist.y};};
    virtual void place(UIRect r);
    static constexpr UIVec mindist{3,2};
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
    template<typename... Args> Layout(Host* h, Args&&... args) : Element(h), elements{std::forward<Args>(args)...} {};
    virtual ~Layout() {};
    virtual UIVec getminimumsize() override = 0;
    void render(Rendering* r) final {};
    bool checkclick(UIVec pos) final {return false;};
    void addelements(std::vector<Element*> els);
    void addelement(Element* el);
    UIVec consolidate();
protected:
    std::vector<Element*> elements;
};

class HBox : public Layout
{
public:
    HBox(Host* h) : Layout(h) {};
    template<typename... Args> HBox(Host* h, Args&&... args) : Layout(h, std::forward<Args>(args)...) {};
    void place(UIRect r) override;
    UIVec getminimumsize() override;
private:
    std::vector<Coord> minwidths;
};

class VBox : public Layout
{
public:
    VBox(Host* h) : Layout(h) {};
    template<typename... Args> VBox(Host* h, Args&&... args) : Layout(h, std::forward<Args>(args)...) {};
    void place(UIRect r) override;
    UIVec getminimumsize() override;
private:
    std::vector<Coord> minheights;
};

} // namespace UI