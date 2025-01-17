#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;
class Route;
class Train;

namespace UI{

class Button;
class Table;
class Dropdown;
class Panel;
class RouteListPanel;

class Element
{
public:
    Element(Panel*);
    virtual ~Element() {};
    virtual bool checkclick(Vec pos, int type);
    virtual void update(int ms) {};
    virtual void render(Rendering*) {};
    virtual SDL_Rect getglobalrect();
    SDL_Rect getlocalrect();
protected:
    Panel* panel;
    SDL_Rect rect = {0,0,100,100};
    InterfaceManager* ui;
    Game* game;
};

class ClickableHost
{
public:
    ClickableHost(InterfaceManager* newui, SDL_Rect newrect);
    virtual bool click(Vec pos, int type);
    virtual void update(int ms);
    virtual void render(Rendering*);
    void addelement(Element*);
    Vec topcorner();
    virtual void erase();
    InterfaceManager& getui();
protected:
    Game* game;
    SDL_Rect rect;
    std::vector<std::unique_ptr<Element>> elements;
    InterfaceManager* ui;
};

class Panel : public ClickableHost
{
public:
    Panel(InterfaceManager* newui, SDL_Rect newrect);
    Panel(InterfaceManager* newui);
    virtual ~Panel();
    virtual void render(Rendering*);
protected:
    template <class T, typename... Args> void createbutton(Args&&... args);
    int xoffset;
    int yoffset;
    int ydist = 10;
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
    TrainPanel(InterfaceManager* newui, SDL_Rect newrect, Train& newtrain);
    ~TrainPanel();
    Train& gettrain() {return train;};
private:
    Train& train;
};

}

class InterfaceManager{
public:
    InterfaceManager(Game*);
    void update(int ms);
    int leftclick(Vec pos);
    void render(Rendering*);
    void addpanel(UI::ClickableHost*);
    void removepanel(UI::ClickableHost*);
    void setdropdown(UI::Dropdown*);
    Game& getgame();
    int getlogicalscale();
private:
    std::vector<std::unique_ptr<UI::ClickableHost>> panels;
    UI::Dropdown* dropdown = nullptr;
    Game* game;
};
