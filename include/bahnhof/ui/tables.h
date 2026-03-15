#pragma once
#include "bahnhof/ui/element.h"
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/economy/payments.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;
class Route;
class RouteManager;
class BuildingType;
class BuildingManager;
class BuildingBuilder;
class Building;
class WagonFactory;
class Entity;
namespace Economy{
    class Stock;
    class Portfolio;
    class Account;
}

namespace UI{

class TableLine;

class Table : public Element
{
public:
    Table(Host*, UIVec minsz, UIVec pos={0,0});
    virtual ~Table();
    bool checkclick(UIVec pos) final;
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
    void leftclick(UIVec pos) final;
    void scroll(UIVec pos, int distance) final;
    virtual void render(Rendering*) override;
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
    ConstructionTable(Host* p, const BuildingManager& manager, 
        const std::vector<BuildingID>& availtypes,
        UIVec pos={0,0}, UIVec minsz={200,100});
private:
    void lineclicked(int index);
    const BuildingManager& buildingmanager;
    const std::vector<BuildingID>& availabletypes;
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

class CompanyInfoTable : public Table // TODO: Either change to StockInfoTable or pass &Company
{
    using Stock = Economy::Stock;
    using Account = Economy::Account;
public:
    CompanyInfoTable(
        Host* p, Stock& c, Account& a, UIVec pos={0,0}, UIVec minsz={150,60}): 
            Table(p, minsz, pos), stock(c), account{a} {};
    void update(int ms);
private:
    Stock& stock;
    Account& account;
};

class OwnersTable : public ClickableTable
{
    using Stock = Economy::Stock;
public:
    OwnersTable(
        Host* p, Stock& s, UIVec pos={0,0}, UIVec minsz={180,80});
    void update(int ms);
private:
    void lineclicked(int index);
    std::vector<Entity*> investors;
    Stock& stock;
};

class AccountInfoTable : public Table
{
    using Account = Economy::Account;
public:
    AccountInfoTable(
        Host* p, Account& a, UIVec pos={0,0}, UIVec minsz={100,40}): 
            Table(p, minsz, pos), account(a) {};
    void update(int ms);
private:
    Account& account;
};

class IncomeTable : public Table
{
public:
    IncomeTable(
        Host* p, const Economy::Typelist& t, 
        UIVec pos={0,0}, UIVec minsz={200,80}): 
            Table(p, minsz, pos), types{t} {};
    void update(int ms);
private:
    const Economy::Typelist& types;
};

class InvestmentsTable : public ClickableTable
{
    using Portfolio = Economy::Portfolio;
public:
    InvestmentsTable(
        Host* p, Portfolio& port, UIVec pos={0,0}, UIVec minsz={180,80});
    void update(int ms);
private:
    void lineclicked(int index);
    std::vector<Economy::Stock*> stocks;
    Portfolio& portfolio;
};

class StocksTable : public ClickableTable
{
    using Stock = Economy::Stock;
public:
    StocksTable(
        Host* p, const std::vector<Stock*>& s, 
        UIVec pos={0,0}, UIVec minsz={300,80});
    void update(int ms);
private:
    void lineclicked(int index);
    const std::vector<Stock*>& stocks;
};

template<typename Possession>
class PossessionsTable : public ClickableTable
{
public:
    PossessionsTable(
        Host* p, const std::vector<Possession*>& s, 
        UIVec pos={0,0}, UIVec minsz={200,300}) : 
            ClickableTable{p, minsz, pos}, possessions{s}
    {}
    void update(int ms);
private:
    void lineclicked(int index);
    const std::vector<Possession*>& possessions;
};

}