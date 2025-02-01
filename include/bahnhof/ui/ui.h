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
    virtual bool checkclick(Vec pos);
    virtual void mousehover(Vec pos, int ms) {};
    virtual void leftclick(Vec pos) {};
    virtual void leftpressed(Vec pos, int mslogic) {};
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

class Text : public Element
{
public:
    Text(Panel* p, std::string t, SDL_Rect r) : Element(p), text(t) {rect = r;};
    void render(Rendering*);
    std::string text;
    SDL_Color color = {0,0,0,255};
};

class TrainIcons : public Element
{
public:
    TrainIcons(Panel* p, SDL_Rect maxarea, Train& t) : Element(p), train(t) {rect = maxarea;};
    void render(Rendering*);
private:
    Train& train;
};

class Host
{
public:
    Host(InterfaceManager* newui, SDL_Rect newrect);
    bool checkclick(Vec pos);
    void mousehover(Vec pos, int ms);
    void click(Vec pos, int type);
    void mousepress(Vec pos, int mslogic, int type);
    void update(int ms);
    virtual void render(Rendering*);
    void addelement(Element*);
    void move(Vec towhattopcorner);
    Vec topcorner();
    virtual void erase();
    InterfaceManager& getui();
protected:
    Element* getelementat(Vec pos);
    Game* game;
    SDL_Rect rect;
    std::vector<std::unique_ptr<Element>> elements;
    InterfaceManager* ui;
};

class Panel : public Host
{
public:
    Panel(InterfaceManager* newui, SDL_Rect newrect);
    Panel(InterfaceManager* newui);
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
    Text* trainnametext;
};

}

class InterfaceManager{
public:
    InterfaceManager(Game*);
    void update(int ms);
    bool mousehover(Vec pos, int ms);
    bool leftclick(Vec pos);
    void leftbuttonup(Vec pos);
    bool leftpressed(Vec pos, int mslogic);
    void render(Rendering*);
    void addpanel(UI::Host*);
    void removepanel(UI::Host*);
    void setdropdown(UI::Dropdown*);
    Game& getgame();
    int getlogicalscale();
    UI::Host* movingwindow = nullptr;
private:
    UI::Host* getpanelat(Vec pos);
    void renderscaleruler(Rendering* r, int leftx, int lefty, int scalelinelength);
    std::vector<std::unique_ptr<UI::Host>> panels;
    UI::Dropdown* dropdown = nullptr;
    Vec movingwindowoffset;
    Game* game;
};
