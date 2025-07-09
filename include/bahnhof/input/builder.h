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

class Builder
{
public:
    Builder(InputManager& owner, Game* newgame);
    virtual ~Builder() {};
    virtual void render(Rendering*) {};
    virtual void leftclickmap(Vec mappos) {};
    virtual void reset() {};
protected:
    Game* game;
    InputManager& input;
    Tracks::Tracksystem& tracksystem;
};

class TrackBuilder : public Builder
{
public:
    TrackBuilder(InputManager& i, Game* g) : Builder(i, g) {};
    void render(Rendering*);
    void leftclickmap(Vec mappos);
    void reset();
private:
    nodeid selectednode = 0;
    Vec trackorigin{0,0};
};

class SignalBuilder : public Builder
{
public:
    SignalBuilder(InputManager& i, Game* g);
    void render(Rendering*);
    void leftclickmap(Vec mappos);
private:
    Icon icon;
};

class BuildingBuilder : public Builder
{
public:
    BuildingBuilder(InputManager& owner, Game* newgame);
private:


};