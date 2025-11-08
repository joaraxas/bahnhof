#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/uimath.h"

class Game;
class Rendering;
class InterfaceManager;

namespace UI{

class Element;
class Host;

class Ownership
{
public:
    ~Ownership();
    void set(Host* newhost);
    bool exists();
    void deletereference();
    void resetreference();
private:
    Host* host = nullptr;
};

class Host
{
public:
    Host(InterfaceManager* newui, UIRect newrect);
    virtual ~Host();
    void mousehover(UIVec pos, int ms);
    void click(UIVec pos, int type);
    void scroll(UIVec pos, int distance);
    void mousepress(UIVec pos, int mslogic, int type);
    virtual void update(int ms);
    virtual void render(Rendering*);
    void addelement(Element*);
    void moveto(UIVec towhattopcorner);
    virtual void erase();
    InterfaceManager& getui();
    UIRect getglobalrect();
    UIRect getlocalrect();
    Ownership* owner = nullptr;
protected:
    Element* getelementat(UIVec pos);
    Game* game;
    UIRect rect;
    std::vector<std::unique_ptr<Element>> elements;
    InterfaceManager* ui;
};

} // namespace UI