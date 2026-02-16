#pragma once
#include "math.h"
#include "bahnhof/input/inputmode.h"
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
class InterfaceManager;
class BuildingManager;
class BuildingType;
class Account;

class Builder : public InputMode
{
public:
    Builder(InputManager& owner, Game* newgame);
    virtual ~Builder() {};
    virtual void render(Rendering*);
    virtual void leftclickmap(Vec mappos);
    virtual void rightclickmap(Vec mappos);
    virtual void leftreleasedmap(Vec mappos);
protected:
    virtual void reset();
    bool canbuild();
    virtual bool canfit() {return true;};
    virtual void build() {};
    float cost;
    Vec anchorpoint{0,0};
    bool droppedanchor = false;
    Angle angle;
    bool angleisset;
    Game* game;
    Tracks::Tracksystem& tracksystem;
private:
    void updateangle(Vec pos);
    InputManager& input;
    Account& myaccount;
};

class TrackBuilder : public Builder
{
public:
    TrackBuilder(InputManager& i, Game* g);
    void render(Rendering*);
    void reset();
private:
    bool canfit();
    void build();
    bool islayingtrack();
    Tracks::Tracksection planconstruction(Vec pos);
    bool buildingfromstartpoint();
    InterfaceManager& ui;
    Vec trackstartpoint{0,0};
    nodeid selectednode = 0;
    State selectedstate;
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
    BuildingBuilder(InputManager& i, Game* g, const BuildingType& b);
    void render(Rendering*);
    void reset();
private:
    bool canfit();
    void build();
    std::unique_ptr<Shape> getplacementat(Vec pos) const;
    const BuildingType& building;
    BuildingManager& buildingmanager;
};