#pragma once
#include "bahnhof/ui/ownership.h"
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/buildings/buildingtypes.h"

class Game;
class Rendering;
class Building;
class Shape;
namespace Tracks{
    struct Tracksection;
}

class BuildingManager
{
public:
    BuildingManager(Game* g);
    void update(int ms);
    void render(Rendering* r);
    bool leftclick(Vec mappos);
    const std::vector<BuildingType>& gettypes() const {return availabletypes;};
    const BuildingType& gettypefromid(BuildingID id) const {return types.at(id);};
    void addbuilding(std::unique_ptr<Building> b);
    bool checkcollision(const Shape& shape);
    bool checkcollision(const Tracks::Tracksection& shape);
    void createconstructionpanel();
private:
    Game* game;
    std::map<BuildingID, BuildingType> types;
    std::vector<BuildingType> availabletypes;
    std::vector<std::unique_ptr<Building>> buildings;
    UI::Ownership constructionpanel;
};