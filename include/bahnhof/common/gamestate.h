#pragma once
#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "bahnhof/common/math.h"
#include "bahnhof/track/state.h"

class Route;
class Wagon;
class Train;
class Building;
class Camera;
class Rendering;
class SpriteManager;
class ResourceManager;
class InputManager;
class RouteManager;
class TrainManager;
class TimeManager;
class InterfaceManager;
class BuildingManager;
class RollingStockManager;
class Gamestate;
namespace Tracks{
    class Tracksystem;
}

class Game
{
public:
    Game();
    ~Game();
    void play();
    void exit();
    std::string gamename;
    TimeManager& gettimemanager() {if(!timer) std::cout<<"no timer"<<std::endl; return *timer;};
    Camera& getcamera() {if(!cam) std::cout<<"no cam"<<std::endl; return *cam;};
    Rendering& getrendering() {if(!rendering) std::cout<<"no rendering"<<std::endl; return *rendering;};
    InterfaceManager& getui() {if(!ui) std::cout<<"no ui"<<std::endl; return *ui;};
    SpriteManager& getsprites() {if(!allsprites) std::cout<<"no allsprites"<<std::endl; return *allsprites;};
    ResourceManager& getresources() {if(!resources) std::cout<<"no resources"<<std::endl; return *resources;};
    Gamestate& getgamestate() {if(!gamestate) std::cout<<"no gamestate"<<std::endl; return *gamestate;};
    InputManager& getinputmanager() {if(!input) std::cout<<"no input"<<std::endl; return *input;};
private:
    std::unique_ptr<TimeManager> timer;
    std::unique_ptr<Camera> cam;
    std::unique_ptr<Rendering> rendering;
    std::unique_ptr<InterfaceManager> ui;
    std::unique_ptr<SpriteManager> allsprites;
    std::unique_ptr<ResourceManager> resources;
    std::unique_ptr<Gamestate> gamestate;
    std::unique_ptr<InputManager> input;
    bool quit;
};

class Gamestate
{
public:
    Gamestate(Game* whatgame);
    ~Gamestate();
    void update(int ms);
    void randommap();
    void inittracks();
    Tracks::Tracksystem& gettracksystems() {if(!tracksystem) std::cout<<"no tracksystem"<<std::endl; return *tracksystem;};
    RouteManager& getrouting() {if(!routing) std::cout<<"no routing"<<std::endl; return *routing;};
    TrainManager& gettrainmanager() {if(!trainmanager) std::cout<<"no trainmanager"<<std::endl; return *trainmanager;};
    BuildingManager& getbuildingmanager() {if(!buildingmanager) std::cout<<"no buildingmanager"<<std::endl; return *buildingmanager;};
    RollingStockManager& getrollingstockmanager() {if(!rollingstockmanager) std::cout<<"no rollingstockmanager"<<std::endl; return *rollingstockmanager;};
    int time = 0;
    float money = 10;
    int revenue = 0;
private:
    Game* game;
    std::unique_ptr<Tracks::Tracksystem> tracksystem;
    std::unique_ptr<RouteManager> routing;
    std::unique_ptr<TrainManager> trainmanager;
    std::unique_ptr<BuildingManager> buildingmanager;
    std::unique_ptr<RollingStockManager> rollingstockmanager;
    //Map* map;
};

class Background
{

};

class Map
{
    Background* background;
};
