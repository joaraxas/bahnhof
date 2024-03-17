#include "bahnhof/resources/resources.h"

struct State;
class Route;
class Wagon;
class Train;
class Building;

class Gamestate
{
public:
    Gamestate();
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
    int revenue = 0;
    State newwagonstate;
};

extern std::vector<std::unique_ptr<Train> > trains;
extern std::vector<std::unique_ptr<Building> > buildings;
