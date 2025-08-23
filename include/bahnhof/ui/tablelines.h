#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<iostream>
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
    virtual void render(Rendering* r, SDL_Rect maxarea, TextStyle style);
    virtual void render(Rendering* r, SDL_Rect maxarea) {render(r, maxarea, Info);};
protected:
    Table* table;
    std::string str;
};

class RouteTableLine : public TableLine
{
public:
    RouteTableLine(Host*, Table*, std::string routename);
    virtual void render(Rendering* r, SDL_Rect maxarea);
};

class OrderTableLine : public TableLine
{
public:
    OrderTableLine(Host*, Table*, bool select, std::string description);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    bool selected = false;
    int orderid;
};

class TrainTableLine : public TableLine
{
public:
    TrainTableLine(Host*, Table*, TrainInfo, TrainManager*);
    void render(Rendering* r, SDL_Rect maxarea);
    TrainInfo info;
private:
    TrainManager* trainmanager;
};

class PurchaseOptionTableLine : public TableLine
{
public:
    PurchaseOptionTableLine(Host* p, Table* t, sprites::name iconname, std::string name, float cost);
    void render(Rendering* r, SDL_Rect maxarea);
protected:
    Icon icon;
    int price;
};


}