#pragma once
#include<vector>
#include "bahnhof/track/state.h"

class Route;
class Wagon;
class Train;
class Tracksystem;
class Building;
class Camera;
class Rendering;
class SpriteManager;
class ResourceManager;
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
    std::string gamename;
    ResourceManager& getresources() {return *resources;};
    Camera& getcamera() {return *cam;};
    TimeManager& gettimemanager() {return *timer;};
    InputManager& getinputmanager() {return *input;};
    Rendering& getrendering() {return *rendering;};
    SpriteManager& getsprites() {return *allsprites;};
    Gamestate& getgamestate() {return *gamestate;};
private:
    ResourceManager* resources;
    Camera* cam;
    TimeManager* timer;
    InputManager* input;
    Rendering* rendering;
    SpriteManager* allsprites;
    Gamestate* gamestate;
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
    Tracksystem& gettracksystems() {return *tracksystem;};
    RouteManager& getrouting() {return *routing;};
    std::vector<Wagon*> wagons;
    std::vector<std::unique_ptr<Building>> buildings;
    std::vector<std::unique_ptr<Train>> trains;
    int time = 0;
    float money = 10;
    int revenue = 0;
    State newwagonstate;
private:
    Game* game;
    RouteManager* routing;
    std::unique_ptr<Tracksystem> tracksystem;
    //Map* map;
};

class Background
{

};

class Map
{
    Background* background;
};
