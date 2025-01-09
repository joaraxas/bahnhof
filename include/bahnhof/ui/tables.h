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
    virtual ~TableLine() {std::cout<<"del tableline"<<std::endl;};
    virtual void render(Rendering* r, SDL_Rect maxarea) {};
    SDL_Rect getglobalrect();
protected:
    Table* table;
};

class TableTextLine : public TableLine
{
public:
    TableTextLine(Panel*, Table*, std::string newstr);
    virtual ~TableTextLine() {std::cout<<"del tabletextline"<<std::endl;};
    virtual void render(Rendering* r, SDL_Rect maxarea);
private:
    std::string str;
};

class RouteTableLine : public TableTextLine, public Button
{
public:
    RouteTableLine(Panel*, Table*, std::string routename, int index);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    virtual void click();
    int routeindex;
};
class NewRouteTableLine : public RouteTableLine
{
public:
    NewRouteTableLine(Panel* p, Table* t);
private:
    void click();
};

class TrainTableLine : public TableLine, public Button
{
public:
    TrainTableLine(Panel*, Table*, TrainInfo, TrainManager*);
    void render(Rendering* r, SDL_Rect maxarea);
private:
    void click();
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
protected:
    std::vector<std::unique_ptr<TableLine>> lines;
};

class MainInfoTable : public Table
{
public:
    MainInfoTable(Panel*, SDL_Rect newrect);
    void update(int ms);
};

class RoutesTable : public Table
{
public:
    RoutesTable(Panel* newpanel, SDL_Rect newrect) : Table(newpanel, newrect) {};
    void update(int ms);
};

class TrainTable : public Table
{
public:
    TrainTable(Panel*, SDL_Rect newrect);
    void update(int ms);
private:
    TrainManager* trainmanager;
};

}