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

namespace UI{

class Dropdown;
class RouteListPanel;
class Text;

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
    MainPanel(InterfaceManager* newui, SDL_Rect newrect);
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
    void update(int ms);
    void render(Rendering* r);
    Train& gettrain() {return train;};
private:
    TrainManager& trainmanager;
    Train& train;
};

}
