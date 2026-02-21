#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/host.h"
#include "bahnhof/ui/ownership.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;
class InputManager;
class Route;
class TrainManager;
class Train;
class Building;
class BuildingBuilder;
class WagonFactory;
class Company;

namespace UI{

class Dropdown;
class RouteListPanel;
class Text;
class Layout;

class Panel : public Host
{
public:
    Panel(InterfaceManager* newui);
    virtual void render(Rendering*);
protected:
    template <class T, typename... Args> T* create(Args&&... args);
    Layout* setlayout(Layout* l);
    Layout* getlayout();
    void applylayout(UIVec minsize);
private:
    Layout* layout{nullptr};
};

class MainPanel : public Panel
{
public:
    MainPanel(InterfaceManager* newui);
    bool usermovable() {return false;};
    void conformtorect(UIRect confrect) override;
};

class RouteListPanel : public Panel
{
public:
    RouteListPanel(InterfaceManager* newui);
};

class RoutePanel : public Panel
{
public:
    RoutePanel(InterfaceManager* newui, Route* editroute);
    void close();
    bool usermovable() {return false;};
    void conformtorect(UIRect confrect) override;
private:
    Route* route;
    InputManager& input;
};

class TrainListPanel : public Panel
{
public:
    TrainListPanel(InterfaceManager* newui);
};

class TrainPanel : public Panel
{
public:
    TrainPanel(InterfaceManager* newui, TrainManager& manager, Train& newtrain);
    Train& gettrain() {return train;};
private:
    TrainManager& trainmanager;
    Train& train;
};

class BuildingConstructionPanel : public Panel
{
public:
    BuildingConstructionPanel(InterfaceManager* newui);
};

class BuildingPanel : public Panel
{
public:
    BuildingPanel(InterfaceManager* newui, Building* b);
    ~BuildingPanel();
private:
    Building* building;
};

class FactoryPanel : public BuildingPanel
{
public:
    FactoryPanel(InterfaceManager* newui, WagonFactory* f);
    ~FactoryPanel();
private:
    WagonFactory* factory;
};

class CompanyPanel : public Panel
{
public:
    CompanyPanel(InterfaceManager* newui, Company& com, std::string& companyname);
private:
    Company& company;
};

}
