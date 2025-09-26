#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/ui/ui.h"
#include "bahnhof/common/math.h"
#include "bahnhof/rollingstock/trainmanager.h"

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
    SDL_Rect getglobalrect();
    virtual void render(Rendering* r, SDL_Rect maxarea, TextStyle style, int xmargin, int ymargin);
    virtual void render(Rendering* r, SDL_Rect maxarea) {render(r, maxarea, Info, 1, 1);};
protected:
    Table* table;
    std::string str;
};

class TrainTableLine : public TableLine
{
public:
    TrainTableLine(Host*, Table*, TrainInfo);
    void render(Rendering* r, SDL_Rect maxarea, TextStyle style, int xmargin, int ymargin);
private:
    TrainInfo info;
};

class PurchaseOptionTableLine : public TableLine
{
public:
    PurchaseOptionTableLine(Host* p, Table* t, sprites::name iconname, std::string name, float cost);
    void render(Rendering* r, SDL_Rect maxarea, TextStyle style, int xmargin, int ymargin);
private:
    Icon icon;
    int price;
};


}