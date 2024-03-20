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
class TimeManager;
class Gamestate;

class Game
{
public:
    Game();
    ~Game();
    void play();
    void exit();
    Gamestate* gamestate;
    ResourceManager* resources;
    Camera* cam;
    TimeManager* timer;
    InputManager* input;
    Rendering* rendering;
private:
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
    //Map* map;
    std::vector<Wagon*> wagons;
    std::vector<std::unique_ptr<Building>> buildings;
    std::vector<std::unique_ptr<Train>> trains;
    std::unique_ptr<Tracksystem> tracksystem;
    int time = 0;
    float money;
    int revenue = 0;
    State newwagonstate;
private:
    Game* game;
};

class Background
{

};

class Map
{
    Background* background;
};
