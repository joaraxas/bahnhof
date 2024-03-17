#pragma once
#include<vector>
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"

class Route;
class Wagon;
class Train;
class Tracksystem;
class Building;
class Camera;
class Rendering;
class InputManager;
class Gamestate;

class Game
{
public:
    Game();
    ~Game();
    Gamestate* gamestate;
    ResourceManager* resources;
    Camera* cam;
    //TimeManager* timer;
    InputManager* input;
    //Map* map;
    //RouteManager* routing;
    Rendering* rendering;
    bool quit;
};

class Gamestate
{
public:
    Gamestate(Game* whatgame);
    ~Gamestate();
    void update(int ms);
    void renderroutes();
    Route* addroute();
    void randommap();
    void initjusttrack();
    void inittrain(State startstate);
    void addtrainstoorphans();
	ResourceManager resources;
    std::vector<Wagon*> wagons;
    std::vector<std::unique_ptr<Route>> routes;
    std::unique_ptr<Tracksystem> tracksystem;
    Route* selectedroute = nullptr;
    int time = 0;
    float money;
    int revenue = 0;
    State newwagonstate;
private:
    Game* game;
};

class TimeManager
{

};

class Background
{

};

class Map
{
    Background* background;
};

class RouteManager
{

};

extern std::vector<std::unique_ptr<Train> > trains;
extern std::vector<std::unique_ptr<Building> > buildings;
