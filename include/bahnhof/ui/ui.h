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

namespace UI{

class Button;

class Panel{
public:
    Panel(InterfaceManager* newui, SDL_Rect newrect);
    virtual ~Panel();
    void erase();
    bool click(Vec pos, int type);
    virtual void render(Rendering*);
    Vec topcorner();
    InterfaceManager& getui();
protected:
    void addbutton(Button* button);
    SDL_Rect rect;
    InterfaceManager* ui;
    Game* game;
    std::vector<std::unique_ptr<Button>> buttons;
    int xoffset;
    int yoffset;
    int ydist = 10;
};

class MainPanel : public Panel
{
public:
    MainPanel(InterfaceManager* newui, SDL_Rect newrect);
    ~MainPanel();
    void render(Rendering*);
};

class RoutePanel : public Panel
{
public:
    RoutePanel(InterfaceManager* newui, SDL_Rect newrect);
    ~RoutePanel();
    void render(Rendering*);
};

class TrainPanel : public Panel
{
public:
    TrainPanel(InterfaceManager* newui, SDL_Rect newrect);
    ~TrainPanel();
    void render(Rendering*);
};

}

class InterfaceManager{
public:
    InterfaceManager(Game*);
    void update(int ms);
    int leftclick(Vec pos);
    void render(Rendering*);
    void addpanel(UI::Panel*);
    void removepanel(UI::Panel*);
    Game& getgame();
    int getlogicalscale();
private:
    std::vector<std::unique_ptr<UI::Panel>> panels;
    Game* game;
};
