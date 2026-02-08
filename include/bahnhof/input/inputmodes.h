#pragma once
#include "bahnhof/common/math.h"
#include "bahnhof/input/inputmode.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/state.h"
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
    InterfaceManager& ui;
    BuildingManager& buildingmanager;
    Tracks::Tracksystem& tracksystem;
};

class RouteMode : public InputMode
{
public:
    RouteMode(Game& g, Route& route);
    void render(Rendering*);
    void leftclickmap(Vec mappos);
    void rightclickmap(Vec mappos);
    void leftreleasedmap(Vec mappos) {};
    void editroute(Route& route);
private:
    InputManager& input;
    RouteManager& routemanager;
    Tracks::Tracksystem& tracksystem;
    InterfaceManager& ui;
    Route* editingroute;
    UI::Ownership routepanel;
};
