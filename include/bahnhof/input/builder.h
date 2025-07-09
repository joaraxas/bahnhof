#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "bahnhof/track/state.h"
#include "bahnhof/graphics/sprite.h"

namespace Tracks{
    class Tracksystem;}

class Game;
class Rendering;
class InputManager;

class TrackBuilder
{
public:
    TrackBuilder(InputManager& owner, Game* newgame);
    void render(Rendering*);
    void leftclickmap(Vec mappos);
    void reset();
private:
    Game* game;
    InputManager& input;
    Tracks::Tracksystem& tracksystem;
    nodeid selectednode = 0;
    Vec trackorigin{0,0};
};

class SignalBuilder
{
public:
    SignalBuilder(InputManager& owner, Game* newgame);
    void render(Rendering*);
    void leftclickmap(Vec mappos);
    void reset();
private:
    Game* game;
    InputManager& input;
    Tracks::Tracksystem& tracksystem;
    Icon icon;
};
