#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/ui/ui.h"

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

class Owner
{
public:
    ~Owner() {deletereference();};
    void set(Host* newhost) {deletereference(); host=newhost; host->owner = this;};
    bool exists() {return host!=nullptr;};
    void deletereference() {if(host) host->erase();}
    void resetreference() {host = nullptr;};
    bool takeoveranyreferenceiffree(Owner* oldowner);
private:
    Host* host = nullptr;
};

class Panel : public Host
{
public:
    Panel(InterfaceManager* newui, SDL_Rect newrect); //TODO: Maybe get rid of this and always handle rect setting in each panel ctor
    Panel(InterfaceManager* newui);
    virtual void render(Rendering*);
protected:
    template <class T, typename... Args> void createbutton(Args&&... args);
    static constexpr int margin_x = 15;
    static constexpr int margin_y = 10;
    int yoffset = 0;
    static constexpr int elementdistance_x = 15;
    static constexpr int elementdistance_y = 5;
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
    RoutePanel(InterfaceManager* newui, SDL_Rect newrect, int routeid, RouteListPanel* rlp);
    ~RoutePanel();
    void erase();
private:
    Route* route;
    RouteListPanel* routelistpanel;
};

class RouteListPanel : public Panel
{
public:
    RouteListPanel(InterfaceManager* newui, SDL_Rect newrect);
    ~RouteListPanel();
    void addroutepanel(int routeindex);
    void deselectroutepanel() {routepanel = nullptr;};
    void erase();
private:
    RoutePanel* routepanel = nullptr;
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
    TrainPanel(InterfaceManager* newui, SDL_Rect newrect, TrainManager& manager, Train& newtrain);
    ~TrainPanel();
    Train& gettrain() {return train;};
private:
    TrainManager& trainmanager;
    Train& train;
};

class BuildingConstructionPanel : public Panel
{
public:
    BuildingConstructionPanel(InterfaceManager* newui, SDL_Rect);
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
