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
class RouteManager;

namespace UI{
class Panel;
class Table;

class TableLine : public Element
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

class RouteTableLine : public TableTextLine
{
public:
    RouteTableLine(Panel*, Table*, std::string routename);
    virtual void render(Rendering* r, SDL_Rect maxarea);
};

class OrderTableLine : public TableTextLine
{
public:
    OrderTableLine(Panel*, Table*, Route*, int orderindex, std::string description);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    Route* route;
    int orderid;
};

class TrainTableLine : public TableLine
{
public:
    TrainTableLine(Panel*, Table*, TrainInfo, TrainManager*);
    void render(Rendering* r, SDL_Rect maxarea);
    TrainInfo info;
private:
    TrainManager* trainmanager;
};

class Table : public Element
{
public:
    Table(Panel*, SDL_Rect newrect);
    virtual ~Table() {std::cout<<"del table"<<std::endl;};
    bool checkclick(Vec pos);
    virtual void render(Rendering*);
protected:
    int getlineindexat(Vec pos);
    std::vector<std::unique_ptr<TableLine>> lines;
};

class Dropdown : public Table
{
public:
    Dropdown(Panel* p, SDL_Rect r);
    virtual void update(int ms) {};
    void render(Rendering* r);
};

class RouteDropdown : public Dropdown
{
public:
    RouteDropdown(Panel* p, SDL_Rect r);
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
    MainInfoTable(Panel*, SDL_Rect newrect);
    void update(int ms);
};

class RouteTable : public Table
{
public:
    RouteTable(Panel* p, SDL_Rect r);
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
    OrderTable(Panel* newpanel, SDL_Rect newrect, Route* myroute) : Table(newpanel, newrect), route(myroute) {}
    void update(int ms);
    void leftclick(Vec pos);
private:
    Route* route;
    std::vector<std::string> descriptions;
    std::vector<int> orderids;
    std::vector<int> numbers;
};

class TrainTable : public Table
{
public:
    TrainTable(Panel*, SDL_Rect newrect);
    void update(int ms);
    void leftclick(Vec pos);
private:
    TrainManager* trainmanager;
    std::vector<TrainInfo> traininfos;
};

}