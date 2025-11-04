#pragma once
#include "bahnhof/ui/element.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/ui/uistyle.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;
class Route;
class RouteManager;

namespace UI{
class Table;

class TableLine : public Element
{
public:
    TableLine(Host*, Table*, std::string);
    virtual ~TableLine() {};
    UIRect getglobalrect();
    virtual void render(Rendering* r, UIRect maxarea, TextStyle style, int xmargin, int ymargin);
    virtual void render(Rendering* r, UIRect maxarea) {render(r, maxarea, Info, 1, 1);};
    void render(Rendering* r) {};
protected:
    Table* table;
    std::string str;
};

class TrainTableLine : public TableLine
{
public:
    TrainTableLine(Host*, Table*, TrainInfo);
    void render(Rendering* r, UIRect maxarea, TextStyle style, int xmargin, int ymargin);
private:
    TrainInfo info;
};

class PurchaseOptionTableLine : public TableLine
{
public:
    PurchaseOptionTableLine(Host* p, Table* t, sprites::name iconname, std::string name, float cost);
    void render(Rendering* r, UIRect maxarea, TextStyle style, int xmargin, int ymargin);
private:
    Icon icon;
    int price;
};


}