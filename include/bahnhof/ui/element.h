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
    virtual UIVec getminimumsize();
    virtual UIRect place(UIRect r);
    UIVec getpadding() {return padding;};
    void setpadding(UIVec s) {padding = s;};
    virtual bool resizable_x() const {return false;};
    virtual bool resizable_y() const {return false;};
protected:
    Host* panel;
    UIRect rect = {0,0,100,100};
    InterfaceManager* ui;
    Game* game;
private:
    UIVec padding{2,2};
};

class Layout : public Element
{
public:
    template<typename... Args> Layout(Host* h, Args&&... args) : 
        Element(h), elements{std::forward<Args>(args)...} 
        {setpadding(UIVec{0,0});};
    virtual ~Layout() {};
    virtual UIVec getminimumsize() override = 0;
    void render(Rendering* r) final {};
    bool checkclick(UIVec pos) final {return false;};
    void addelements(std::vector<Element*> els);
    virtual void addelement(Element* el);
protected:
    std::vector<Element*> elements;
};

class HBox : public Layout
{
public:
    template<typename... Args> HBox(Host* h, Args&&... args) : 
        Layout(h, std::forward<Args>(args)...) 
        {getminimumsize();};
    UIRect place(UIRect r) override;
    UIVec getminimumsize() override;
    void addelement(Element* el) override;
    bool resizable_x() const override {return numresizableelements>0;};
private:
    std::vector<Coord> minwidths;
    int numresizableelements = 0;
};

class VBox : public Layout
{
public:
    template<typename... Args> VBox(Host* h, Args&&... args) : 
        Layout(h, std::forward<Args>(args)...) 
        {getminimumsize();};
    UIRect place(UIRect r) override;
    UIVec getminimumsize() override;
    void addelement(Element* el) override;
    bool resizable_y() const override {return numresizableelements>0;};
private:
    std::vector<Coord> minheights;
    int numresizableelements = 0;
};

} // namespace UI