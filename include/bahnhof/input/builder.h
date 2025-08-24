#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/shape.h"
#include "bahnhof/track/state.h"
#include "bahnhof/graphics/sprite.h"

namespace Tracks{
    class Tracksystem;
    struct Tracksection;
}

class Game;
class Rendering;
class InputManager;
class BuildingManager;
class BuildingType;

class Builder
{
public:
    Builder(InputManager& owner, Game* newgame);
    virtual ~Builder() {};
    virtual void render(Rendering*);
    virtual void leftclickmap(Vec mappos);
    virtual void leftreleasedmap(Vec mappos);
    virtual void reset();
protected:
    bool canbuild();
    virtual bool canfit() {return true;};
    virtual void build() {};
    void updateangle(Vec pos);
    Game* game;
    InputManager& input;
    Tracks::Tracksystem& tracksystem;
    Vec anchorpoint{0,0};
    float angle;
    bool droppedanchor = false;
    float cost;
};

class TrackBuilder : public Builder
{
public:
    TrackBuilder(InputManager& i, Game* g) : Builder(i, g) {};
    void render(Rendering*);
    void reset();
private:
    void build();
    Tracks::Tracksection planconstruction(Vec pos);
    bool originwasset();
    Vec origin{0,0};
    nodeid selectednode = 0;
};

class SignalBuilder : public Builder
{
public:
    SignalBuilder(InputManager& i, Game* g);
    void render(Rendering*);
private:
    bool canfit();
    void build();
    Icon icon;
};

class BuildingBuilder : public Builder
{
public:
    BuildingBuilder(InputManager& i, Game* g) : Builder(i, g), buildingmanager(g->getgamestate().getbuildingmanager()) {};
    void render(Rendering*);
    void reset();
    void setbuildingtype(const BuildingType& b);
private:
    bool canfit() {return Builder::canfit();};
    void build();
    std::unique_ptr<Shape> getplacementat(Vec pos);
    const BuildingType* building = nullptr;
    BuildingManager& buildingmanager;
};