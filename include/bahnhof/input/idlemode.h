#pragma once
#include "bahnhof/common/math.h"
#include "bahnhof/input/inputmode.h"

namespace Tracks{
    class Tracksystem;
}

class Game;
class Train;
class Rendering;
class TrainManager;
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
