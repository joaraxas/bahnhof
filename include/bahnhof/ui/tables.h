#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"
#include "bahnhof/rollingstock/train.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{
class Panel;

class Element
{
public:
    Element(Panel*);
    virtual ~Element() {std::cout<<"del element"<<std::endl;};
    virtual void update(int ms) {};
    virtual void render(Rendering*) {};
    SDL_Rect getglobalrect();
    SDL_Rect getlocalrect();
protected:
    Panel* panel;
    SDL_Rect rect = {0,0,100,100};
    InterfaceManager* ui;
    Game* game;
};

class TrainTable : public Element
{
public:
    TrainTable(Panel*, SDL_Rect newrect);
    void update(int ms);
    void render(Rendering*);
protected:
    TrainManager* trainmanager;
    std::vector<TrainInfo> traininfos;
	std::vector<std::string> trainstrings;
	std::vector<float> trainspeeds;
};

}