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
    TableLine(Host*, Table*);
    virtual ~TableLine() {};
    virtual void render(Rendering* r, SDL_Rect maxarea) {};
    SDL_Rect getglobalrect();
protected:
    Table* table;
};

class TableTextLine : public TableLine
{
public:
    TableTextLine(Host*, Table*, std::string newstr);
    virtual ~TableTextLine() {};
    virtual void render(Rendering* r, SDL_Rect maxarea, TextStyle style);
    virtual void render(Rendering* r, SDL_Rect maxarea) {render(r, maxarea, Info);};
private:
    std::string str;
};

class RouteTableLine : public TableTextLine
{
public:
    RouteTableLine(Host*, Table*, std::string routename);
    virtual void render(Rendering* r, SDL_Rect maxarea);
};

class OrderTableLine : public TableTextLine
{
public:
    OrderTableLine(Host*, Table*, bool select, int orderindex, std::string description);
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

class ConstructionTableLine : public TableLine
{
public:
    ConstructionTableLine(Host*, Table*, std::string newname);
    void render(Rendering* r, SDL_Rect maxarea);
protected:
    Icon icon;
    std::string name;
    int price;
};


}