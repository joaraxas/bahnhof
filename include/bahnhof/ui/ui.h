#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"

class Panel;
class InterfaceManager;
class Game;
class Gamestate;
class Rendering;

class Button{
    Button(Panel&, Vec newpos);
    Panel* panel;
    Vec pos;
};

class Panel{
public:
    Panel(InterfaceManager* newui, SDL_Rect newrect);
    bool click(Vec pos, int type);
    void render(Rendering*);
private:
    InterfaceManager* ui;
    SDL_Rect rect;
    std::vector<std::unique_ptr<Button>> buttons;
};

class InterfaceManager{
public:
    InterfaceManager(Game*);
    void update(int ms);
    int leftclick(Vec pos);
    void render(Rendering*);
private:
    std::vector<std::unique_ptr<Panel>> panels;
    Game* game;
};