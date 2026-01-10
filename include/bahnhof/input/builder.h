#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/shape.h"
#include "bahnhof/track/state.h"
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/ui/ownership.h"

namespace Tracks{
    class Tracksystem;
    struct Tracksection;
}

class Game;
class Rendering;
class TrainManager;
class InputManager;
class BuildingManager;
class BuildingType;

namespace UI{
    class BuildingConstructionPanel;
}

class InputMode
{
public:
    virtual ~InputMode() {};
    virtual void render(Rendering*) = 0;
    virtual void leftclickmap(Vec mappos) = 0;
    virtual void rightclickmap(Vec mappos) = 0;
    virtual void leftreleasedmap(Vec mappos) = 0;
};

class IdleMode : public InputMode
{
public:
    IdleMode(Game& g);
    virtual void render(Rendering*) {};
    virtual void leftclickmap(Vec mappos);
    virtual void rightclickmap(Vec mappos) {};
    virtual void leftreleasedmap(Vec mappos) {};
private:
    void selecttrain(Train* train);
    TrainManager& trainmanager;
    BuildingManager& buildingmanager;
    Tracks::Tracksystem& tracksystem;
};

class DeleteMode : public InputMode
{
public:
    DeleteMode(Game& g);
    virtual void render(Rendering*);
    virtual void leftclickmap(Vec mappos);
    virtual void rightclickmap(Vec mappos);
    virtual void leftreleasedmap(Vec mappos) {};
private:
    InputManager& input;
    BuildingManager& buildingmanager;
    Tracks::Tracksystem& tracksystem;
};

class RouteMode : public InputMode
{
public:
    RouteMode(Game& g);
private:
    InputManager& input;
    RouteManager& routemanager;
    Route* route;
};

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
};

class TrackBuilder : public Builder
{
public:
    TrackBuilder(InputManager& i, Game* g) : Builder(i, g) {};
    void render(Rendering*);
    void reset();
private:
    bool canfit();
    void build();
    bool islayingtrack();
    Tracks::Tracksection planconstruction(Vec pos);
    bool buildingfromstartpoint();
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
    BuildingBuilder(InputManager& i, Game* g);
    void rightclickmap(Vec mappos);
    void render(Rendering*);
    void reset();
    void setbuildingtype(const BuildingType* b);
private:
    bool canfit();
    void build();
    std::unique_ptr<Shape> getplacementat(Vec pos);
    const BuildingType* building = nullptr;
    UI::Ownership panel;
    BuildingManager& buildingmanager;
};