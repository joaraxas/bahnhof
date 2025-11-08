#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/host.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;
class Route;
class TrainManager;
class Train;
class Building;
class WagonFactory;

namespace UI{

class Dropdown;
class RouteListPanel;
class Text;

class Panel : public Host
{
public:
    Panel(InterfaceManager* newui, UIRect newrect); //TODO: Maybe get rid of this and always handle rect setting in each panel ctor
    Panel(InterfaceManager* newui);
    virtual void render(Rendering*);
protected:
    template <class T, typename... Args> Element* createbutton(Args&&... args);
    static constexpr Coord margin_x = 15;
    static constexpr Coord margin_y = 10;
    Coord yoffset = 0;
    static constexpr Coord elementdistance_x = 15;
    static constexpr Coord elementdistance_y = 5;
};

class MainPanel : public Panel
{
public:
    MainPanel(InterfaceManager* newui);
    ~MainPanel();
};

class RoutePanel : public Panel
{
public:
    RoutePanel(InterfaceManager* newui, UIRect newrect, int routeid);
    ~RoutePanel();
    void erase();
private:
    Route* route;
};

class RouteListPanel : public Panel
{
public:
    RouteListPanel(InterfaceManager* newui, UIRect newrect);
    ~RouteListPanel();
    void erase();
    void addroutepanel(int routeindex);
private:
    std::unique_ptr<UI::Ownership> routepanelref;
};

class TrainListPanel : public Panel
{
public:
    TrainListPanel(InterfaceManager* newui);
    ~TrainListPanel();
};

class TrainPanel : public Panel
{
public:
    TrainPanel(InterfaceManager* newui, UIRect newrect, TrainManager& manager, Train& newtrain);
    ~TrainPanel();
    Train& gettrain() {return train;};
private:
    TrainManager& trainmanager;
    Train& train;
};

class BuildingConstructionPanel : public Panel
{
public:
    BuildingConstructionPanel(InterfaceManager* newui, UIRect);
    ~BuildingConstructionPanel();
    void erase() override;
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
    void render(Rendering* r);
private:
    WagonFactory* factory;
};

}
