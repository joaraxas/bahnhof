#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "bahnhof/track/state.h"

namespace Tracks{
    class Tracksystem;}

class Game;
class Rendering;
class Train;
class Route;

const int gasbutton = SDL_SCANCODE_RIGHT;
const int brakebutton = SDL_SCANCODE_LEFT;
const int gearbutton = SDL_SCANCODE_LSHIFT;
const int numberbuttons[10] = {SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9};
const int loadbutton = SDL_SCANCODE_L;
const int unloadbutton = SDL_SCANCODE_U;
const int couplebutton = SDL_SCANCODE_G;
const int leftpanbutton = SDL_SCANCODE_A;
const int rightpanbutton = SDL_SCANCODE_D;
const int uppanbutton = SDL_SCANCODE_W;
const int downpanbutton = SDL_SCANCODE_S;

class InputManager
{
public:
    InputManager(Game* whatgame);
    void handle(int ms, int mslogic);
    void render(Rendering*, Tracks::Tracksystem&);
    Vec screenmousepos();
    Vec mapmousepos();
    bool keyispressed(const int scancode);
    bool isleftmousepressed();
    void selecttrain(Train* train);
    Train* getselectedtrain() {return selectedtrain;};
    void editroute(Route* route);
    void placesignal();
    void placetrack();
private:
    Game* game;
    const Uint8* keys;
    bool placingsignal = false;
    bool placingtrack = false;
    Train* selectedtrain = nullptr;
    nodeid selectednode = 0;
    Vec trackorigin;
    Route* editingroute = nullptr;
};