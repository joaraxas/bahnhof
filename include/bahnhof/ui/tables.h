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
class BuildingBuilder;
class WagonFactory;

namespace UI{

class TableLine;

class Table : public Element
{
public:
    Table(Host*, UIVec minsz, UIVec pos={0,0});
    virtual ~Table();
    bool checkclick(UIVec pos) override;
    virtual void render(Rendering*) override;
    virtual UIVec getminimumsize() override;
    UIRect place(UIRect r) override;
    bool resizable_x() const override {return true;};
    bool resizable_y() const override {return true;};
protected:
    std::vector<std::unique_ptr<TableLine>> lines;
private:
    const UIVec minsize;
};

class ClickableTable : public Table
{
public:
    ClickableTable(Host* h, UIVec minsz, UIVec pos={0,0}) : 
        Table(h, minsz, pos) {};
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
    Dropdown(Host* p, UIVec pos, UIVec minsz);
    virtual void update(int ms) {};
    void render(Rendering* r);
};

class RouteDropdown : public Dropdown
{
public:
    RouteDropdown(Host* p, UIVec pos, UIVec minsz={150,100});
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
    MainInfoTable(Host*, UIVec pos={0,0}, UIVec minsz={60,100});
    void update(int ms);
};

class RouteTable : public ClickableTable
{
public:
    RouteTable(Host* p, UIVec pos={0,0}, UIVec minsz={150,120});
    void update(int ms);
private:
    void lineclicked(int index);
    InputManager& input;
	RouteManager& routing;
    std::vector<std::string> names;
    std::vector<int> ids;
};

class OrderTable : public ClickableTable
{
public:
    OrderTable(Host* newpanel, Route* myroute, UIVec pos={0,0}, UIVec minsz={150,100}) : 
        ClickableTable(newpanel, minsz, pos), route(myroute) {};
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
    TrainOrderTable(Host* p, Train& t, UIVec pos={0,0}, UIVec minsz={200,150}) : 
        OrderTable(p, nullptr, pos, minsz), train(t) {};
    void update(int ms);
private:
    void lineclicked(int index);
    Train& train;
};

class TrainTable : public ClickableTable
{
public:
    TrainTable(Host*, UIVec pos={0,0}, UIVec minsz={300,100});
    void update(int ms);
private:
    void lineclicked(int index);
    TrainManager* trainmanager;
    std::vector<TrainInfo> traininfos;
};

class TrainInfoTable : public Table
{
public:
    TrainInfoTable(Host* p, Train& t, UIVec pos={0,0}, UIVec minsz={80,150}): 
        Table(p, minsz, pos), train(t) {};
    void update(int ms);
private:
    Train& train;
};

class ConstructionTable : public ClickableTable
{
public:
    ConstructionTable(Host* p, BuildingBuilder& b, UIVec pos={0,0}, UIVec minsz={200,100});
private:
    void lineclicked(int index);
    BuildingBuilder& builder;
    const std::vector<BuildingType>& buildingtypes;
};

class WagonTable : public ClickableTable
{
public:
    WagonTable(Host* p, WagonFactory& f, UIVec pos={0,0}, UIVec minsz={150,80});
private:
    void lineclicked(int index);
    InputManager& input;
    WagonFactory& factory;
};

}