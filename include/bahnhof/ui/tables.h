#pragma once
#include "bahnhof/ui/element.h"
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/buildings/buildingtypes.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;
class Route;
class RouteManager;
class BuildingType;
class WagonFactory;

namespace UI{

class TableLine;

class Table : public Element
{
public:
    Table(Host*, UIRect newrect);
    virtual ~Table();
    bool checkclick(UIVec pos) override;
    virtual void render(Rendering*) override;
    virtual UIVec getminimumsize() override;
    void place(UIRect r) override;
protected:
    std::vector<std::unique_ptr<TableLine>> lines;
private:
    const UIVec minsize;
};

class ClickableTable : public Table
{
public:
    ClickableTable(Host* h, UIRect r) : Table(h, r) {};
    void leftclick(UIVec pos);
    void scroll(UIVec pos, int distance);
    virtual void render(Rendering*);
protected:
    virtual void lineclicked(int index) {selectedlineindex = index;};
    int selectedlineindex = -1;
private:
    int getlineindexat(UIVec pos);
    int toplineindex = 0;
    Coord linescrolloffset = 0;
};

class Dropdown : public ClickableTable
{
public:
    Dropdown(Host* p, UIRect r);
    virtual void update(int ms) {};
    void render(Rendering* r);
};

class RouteDropdown : public Dropdown
{
public:
    RouteDropdown(Host* p, UIRect r);
    void update(int ms);
private:
    void lineclicked(int index);
	RouteManager& routing;
    std::vector<std::string> names;
    std::vector<int> ids;
};

class MainInfoTable : public Table
{
public:
    MainInfoTable(Host*, UIRect newrect);
    void update(int ms);
};

class RouteTable : public ClickableTable
{
public:
    RouteTable(Host* p, UIRect r);
    void update(int ms);
private:
    void lineclicked(int index);
	RouteManager& routing;
    std::vector<std::string> names;
    std::vector<int> ids;
};

class OrderTable : public ClickableTable
{
public:
    OrderTable(Host* newpanel, UIRect newrect, Route* myroute) : 
        ClickableTable(newpanel, newrect), route(myroute) {};
    virtual void update(int ms);
    void render(Rendering* r);
protected:
    void lineclicked(int index);
    Route* route;
    std::vector<std::string> descriptions;
    std::vector<int> orderids;
    std::vector<int> numbers;
};

class TrainOrderTable : public OrderTable
{
public:
    TrainOrderTable(Host* p, UIRect r, Train& t) : 
        OrderTable(p, r, nullptr), train(t) {};
    void update(int ms);
private:
    void lineclicked(int index);
    Train& train;
};

class TrainTable : public ClickableTable
{
public:
    TrainTable(Host*, UIRect newrect);
    void update(int ms);
private:
    void lineclicked(int index);
    TrainManager* trainmanager;
    std::vector<TrainInfo> traininfos;
};

class TrainInfoTable : public Table
{
public:
    TrainInfoTable(Host* p, UIRect r, Train& t): Table(p, r), train(t) {};
    void update(int ms);
private:
    Train& train;
};

class ConstructionTable : public ClickableTable
{
public:
    ConstructionTable(Host* p, UIRect r);
private:
    void lineclicked(int index);
    InputManager& input;
    const std::vector<BuildingType>& buildingtypes;
};

class WagonTable : public ClickableTable
{
public:
    WagonTable(Host* p, UIRect r, WagonFactory& f);
private:
    void lineclicked(int index);
    InputManager& input;
    WagonFactory& factory;
};

}