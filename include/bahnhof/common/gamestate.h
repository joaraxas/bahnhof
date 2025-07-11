#pragma once
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
    TimeManager& gettimemanager() {return *timer;};
    Camera& getcamera() {return *cam;};
    Rendering& getrendering() {return *rendering;};
    InterfaceManager& getui() {return *ui;};
    SpriteManager& getsprites() {return *allsprites;};
    ResourceManager& getresources() {return *resources;};
    Gamestate& getgamestate() {return *gamestate;};
    InputManager& getinputmanager() {return *input;};
    BuildingManager& getbuildingmanager() {return *buildings;};
private:
    std::unique_ptr<TimeManager> timer;
    std::unique_ptr<Camera> cam;
    std::unique_ptr<Rendering> rendering;
    std::unique_ptr<InterfaceManager> ui;
    std::unique_ptr<SpriteManager> allsprites;
    std::unique_ptr<ResourceManager> resources;
    std::unique_ptr<Gamestate> gamestate;
    std::unique_ptr<InputManager> input;
    std::unique_ptr<BuildingManager> buildings;
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
    Tracks::Tracksystem& gettracksystems() {return *tracksystem;};
    RouteManager& getrouting() {return *routing;};
    TrainManager& gettrainmanager() {return *trainmanager;};
    std::vector<std::unique_ptr<Building>> buildings;
    int time = 0;
    float money = 10;
    int revenue = 0;
    State newwagonstate; //TODO: remove this as it is not protected from track splitting
private:
    Game* game;
    std::unique_ptr<Tracks::Tracksystem> tracksystem;
    std::unique_ptr<RouteManager> routing;
    std::unique_ptr<TrainManager> trainmanager;
    //Map* map;
};

class Background
{

};

class Map
{
    Background* background;
};
