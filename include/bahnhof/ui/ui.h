#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"

class Panel;
class Game;
class Gamestate;
class Rendering;

class Button{
    Button();
    Panel* panel;
    Vec pos;
};

class Panel{
    Panel();
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