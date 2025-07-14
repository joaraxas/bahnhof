#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "bahnhof/common/shape.h"
#include "bahnhof/track/state.h"
#include "bahnhof/graphics/sprite.h"

namespace Tracks{
    class Tracksystem;}

class Game;
class Rendering;
class InputManager;
class BuildingType;

class Builder
{
public:
    Builder(InputManager& owner, Game* newgame);
    virtual ~Builder() {};
    virtual void render(Rendering*) {};
    virtual void leftclickmap(Vec mappos);
    virtual void reset() {};
protected:
    bool canbuild(Vec pos);
    virtual bool canfit(Vec pos) {return true;};
    virtual void build(Vec pos) {};
    float cost;
    Game* game;
    InputManager& input;
    Tracks::Tracksystem& tracksystem;
};

class TrackBuilder : public Builder
{
public:
    TrackBuilder(InputManager& i, Game* g) : Builder(i, g) {};
    void render(Rendering*);
    void reset();
private:
    void build(Vec pos);
    nodeid selectednode = 0;
    Vec trackorigin{0,0};
};

class SignalBuilder : public Builder
{
public:
    SignalBuilder(InputManager& i, Game* g);
    void render(Rendering*);
private:
    bool canfit(Vec pos);
    void build(Vec pos);
    Icon icon;
};

class BuildingBuilder : public Builder
{
public:
    BuildingBuilder(InputManager& i, Game* g) : Builder(i, g) {};
    void render(Rendering*);
    void reset();
    void setbuildingtype(const BuildingType& b);
private:
    bool canfit();
    void build(Vec pos);
    std::unique_ptr<Shape> getplacementat(Vec pos);
    const BuildingType* building = nullptr;
};