#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/buildings/buildingtypes.h"

class Game;
class Rendering;
class Building;
class Shape;

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
    bool checkcollision(Shape* shape);
private:
    Game* game;
    std::map<BuildingID, BuildingType> types;
    std::vector<BuildingType> availabletypes;
    std::vector<std::unique_ptr<Building>> buildings;
};