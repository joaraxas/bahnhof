#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/ui/buttons.h"
#include "bahnhof/common/math.h"
#include "bahnhof/rollingstock/train.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{
class Panel;
class Table;

class TableLine : virtual public Element
{
public:
    TableLine(Panel*, Table*);
    virtual ~TableLine() {};
    virtual void render(Rendering* r, SDL_Rect maxarea) {};
    SDL_Rect getglobalrect();
protected:
    Table* table;
};

class TableTextLine : public TableLine
{
public:
    TableTextLine(Panel*, Table*, std::string newstr);
    virtual ~TableTextLine() {};
    virtual void render(Rendering* r, SDL_Rect maxarea, SDL_Color color);
    virtual void render(Rendering* r, SDL_Rect maxarea) {render(r, maxarea, {0,0,0,255});};
private:
    std::string str;
};

class RouteTableLine : public TableTextLine, public Button
{
public:
    RouteTableLine(Panel*, Table*, std::string routename, int index);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    virtual void click(Vec mousepos);
    int routeindex;
};
class NewRouteTableLine : public RouteTableLine
{
public:
    NewRouteTableLine(Panel* p, Table* t);
private:
    void click(Vec mousepos);
};

class OrderTableLine : public TableTextLine, public Button
{
public:
    OrderTableLine(Panel*, Table*, Route*, int orderindex, std::string description);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    void click(Vec mousepos);
    Route* route;
    int orderid;
};

class TrainTableLine : public TableLine, public Button
{
public:
    TrainTableLine(Panel*, Table*, TrainInfo, TrainManager*);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    void click(Vec mousepos);
    TrainInfo info;
    TrainManager* trainmanager;
};

class Table : public Element
{
public:
    Table(Panel*, SDL_Rect newrect);
    virtual ~Table() {std::cout<<"del table"<<std::endl;};
    bool checkclick(Vec pos, int type);
    virtual void render(Rendering*);
    std::vector<std::unique_ptr<TableLine>> lines;
protected:
};

class Dropdown : public Table, public ClickableHost
{
public:
    Dropdown(InterfaceManager* _ui, SDL_Rect r, Panel* p) : ClickableHost(_ui, r), Table(p, r) {};
};

class MainInfoTable : public Table
{
public:
    MainInfoTable(Panel*, SDL_Rect newrect);
    void update(int ms);
};

class RouteListTable : public Table
{
public:
    RouteListTable(Panel* newpanel, SDL_Rect newrect) : Table(newpanel, newrect) {};
    void update(int ms);
};

class RouteTable : public Table
{
public:
    RouteTable(Panel* newpanel, SDL_Rect newrect, Route* myroute) : Table(newpanel, newrect), route(myroute) {}
    void update(int ms);
private:
    Route* route;
};

class TrainListTable : public Table
{
public:
    TrainListTable(Panel*, SDL_Rect newrect);
    void update(int ms);
private:
    TrainManager* trainmanager;
};

}