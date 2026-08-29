#pragma once
#include "math.h"
#include "bahnhof/input/inputmode.h"
#include "bahnhof/common/shape.h"
#include "bahnhof/track/state.h"
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/economy/money.h"
#include "bahnhof/economy/account.h"
#include "bahnhof/economy/control.h"

namespace Tracks{
    class Tracksystem;
    struct Tracksection;
}

class Game;
class Camera;
class Rendering;
class InputManager;
class InterfaceManager;
class BuildingManager;
class BuildingType;
class Building;
namespace Economy{
    class Account;
}
class Storage;

class Orientor
{
public:
    Orientor(Camera& c) : cam{c} {};
    void startpoint(Vec pos);
    void endpoint(Vec pos);
    void reset();
    bool anchordropped() const {return droppedanchor;}
    Angle get() const {return angle;}
    Vec getanchor() const {return anchorpoint;}
    bool isanglevalid() const {return valid;}
private:
    void updateangle(Vec pos);
    Camera& cam;
    Vec anchorpoint{0,0};
    bool droppedanchor{false};
    Angle angle{};
    bool valid{false};
};

class Builder : public InputMode
{
public:
    Builder(InputManager& inp, Game* newgame);
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
    Economy::Money cost;
    Game* game;
    Tracks::Tracksystem& tracksystem;
    Orientor orientor;
private:
    InputManager& input;
};

class TrackBuilder : public Builder
{
public:
    TrackBuilder(InputManager& i, Game* g);
    void render(Rendering*);
private:
    void reset();
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
    using BuildingOwner = Economy::Control<Building>;
public:
    BuildingBuilder(InputManager& i, Game* g, const BuildingType& b);
    void render(Rendering*);
private:
    bool canfit();
    void build();
    std::unique_ptr<Shape> getplacementat(Vec pos) const;
    const BuildingType& building;
    BuildingManager& buildingmanager;
};


class StorageBuilder : public Builder
{
    using StorageOwner = Economy::Control<Storage>;
public:
    StorageBuilder(InputManager& i, Game* g);
    void render(Rendering*);
private:
    void reset();
    bool canfit();
    void build();

};