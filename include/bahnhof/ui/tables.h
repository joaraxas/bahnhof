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

class Table : public Element
{
public:
    Table(Host*, SDL_Rect newrect);
    virtual ~Table() {std::cout<<"del table"<<std::endl;};
    bool checkclick(Vec pos);
    void scroll(Vec pos, int distance);
    virtual void render(Rendering*);
protected:
    int getlineindexat(Vec pos);
    std::vector<std::unique_ptr<TableLine>> lines;
    int toplineindex = 0;
    int linescrolloffset = 0;
};

class Dropdown : public Table
{
public:
    Dropdown(Host* p, SDL_Rect r);
    virtual void update(int ms) {};
    void render(Rendering* r);
};

class RouteDropdown : public Dropdown
{
public:
    RouteDropdown(Host* p, SDL_Rect r);
    void update(int ms);
    void leftclick(Vec pos);
private:
	RouteManager& routing;
    std::vector<std::string> names;
    std::vector<int> ids;
};

class MainInfoTable : public Table
{
public:
    MainInfoTable(Host*, SDL_Rect newrect);
    void update(int ms);
};

class RouteTable : public Table
{
public:
    RouteTable(Host* p, SDL_Rect r);
    void update(int ms);
    void leftclick(Vec pos);
private:
	RouteManager& routing;
    std::vector<std::string> names;
    std::vector<int> ids;
};

class OrderTable : public Table
{
public:
    OrderTable(Host* newpanel, SDL_Rect newrect, Route* myroute) : Table(newpanel, newrect), route(myroute) {};
    virtual void update(int ms);
    virtual void leftclick(Vec pos);
protected:
    Route* route;
    std::vector<std::string> descriptions;
    std::vector<int> orderids;
    std::vector<int> numbers;
};

class TrainOrderTable : public OrderTable
{
public:
    TrainOrderTable(Host* p, SDL_Rect r, Train& t) : OrderTable(p, r, nullptr), train(t) {};
    void update(int ms);
    void leftclick(Vec pos);
private:
    Train& train;
};

class TrainTable : public Table
{
public:
    TrainTable(Host*, SDL_Rect newrect);
    void update(int ms);
    void leftclick(Vec pos);
private:
    TrainManager* trainmanager;
    std::vector<TrainInfo> traininfos;
};

class TrainInfoTable : public Table
{
public:
    TrainInfoTable(Host* p, SDL_Rect r, Train& t): Table(p, r), train(t) {};
    void update(int ms);
private:
    Train& train;
};

}