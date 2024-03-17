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
class RouteManager;
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
    Rendering* rendering;
    bool quit;
};

class Gamestate
{
public:
    Gamestate(Game* whatgame);
    ~Gamestate();
    void update(int ms);
    void randommap();
    void initjusttrack();
    void inittrain(State startstate);
    void addtrainstoorphans();
    RouteManager* routing;
    std::vector<Wagon*> wagons;
    std::unique_ptr<Tracksystem> tracksystem;
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

extern std::vector<std::unique_ptr<Train> > trains;
extern std::vector<std::unique_ptr<Building> > buildings;
