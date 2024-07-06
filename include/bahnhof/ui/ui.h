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
    ~Panel();
    void erase();
    bool click(Vec pos, int type);
    void render(Rendering*);
    Vec topcorner();
    InterfaceManager& getui();
private:
    SDL_Rect rect;
    InterfaceManager* ui;
    std::vector<std::unique_ptr<Button>> buttons;
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
private:
    std::vector<std::unique_ptr<UI::Panel>> panels;
    Game* game;
};
