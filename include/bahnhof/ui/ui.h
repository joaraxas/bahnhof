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
public:
    Button(Panel*, Vec newpos);
    bool click(Vec pos, int type);
    void render(Rendering*);
    SDL_Rect getabsrect();
private:
    Vec pos;
    Panel* panel;
    SDL_Rect rect;
    //InterfaceManager* ui;
};

class Panel{
public:
    Panel(InterfaceManager* newui, SDL_Rect newrect);
    void erase();
    bool click(Vec pos, int type);
    void render(Rendering*);
    Vec topcorner();
private:
    SDL_Rect rect;
    InterfaceManager* ui;
    std::vector<std::unique_ptr<Button>> buttons;
};

class InterfaceManager{
public:
    InterfaceManager(Game*);
    void update(int ms);
    int leftclick(Vec pos);
    void render(Rendering*);
    void addpanel(Panel*);
    void removepanel(Panel*);
private:
    std::vector<std::unique_ptr<Panel>> panels;
    Game* game;
};