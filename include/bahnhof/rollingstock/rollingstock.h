#include "bahnhof/track/track.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/utils.h"

class Storage;
class Route;
class Order;

class Wagon
{
public:
    Wagon(Tracksystem& newtracksystem, State trackstate, std::string path, std::string iconpath="");
    void travel(float pixels);
    virtual void update(int ms);
    virtual void render();
    virtual State frontendstate();
    virtual State backendstate();
    virtual int loadwagon(resourcetype type, int amount);
    virtual int unloadwagon(resourcetype* type);
    virtual float getpower();
    Train* train;
    Vec pos;
    bool alignedforward = true;
    bool hasdriver = false;
    int w;
    State state; //should be protected
protected:
    Tracksystem* tracksystem;
    int h;
    int iconw, iconh;
    float imageangle = 0;
    SDL_Texture* tex;
    SDL_Texture* icontex;
    //ResourceManager* allresources;
private:
    resourcetype loadedresource = none;
    int loadamount = 0;
    int maxamount = 1;
};

class Locomotive : public Wagon
{
public:
    Locomotive(Tracksystem& newtracksystem, State trackstate);
    void render();
    void update(int ms);
    int loadwagon(resourcetype type, int amount);
    int unloadwagon(resourcetype* type);
    float getpower();
private:
    float P[2] = {0.2,0.2};
    float maxspeed[2] = {90,180};
    int imagenumber = 4;
    float imageindex = 0;
    float imagespeed = 2;
};

class Openwagon : public Wagon
{
public:
    Openwagon(Tracksystem& newtracksystem, State trackstate);
    int loadwagon(resourcetype type, int amount);
};

class Tankwagon : public Wagon
{
public:
    Tankwagon(Tracksystem& newtracksystem, State trackstate);
    int loadwagon(resourcetype type, int amount);
};


class Train
{
public:
    Train(Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed);
    void getinput(int ms);
    void update(int ms);
    void checkcollision(int ms, Train* train);
    void render();
    bool perform(int ms);
    void proceed();
    bool gas(int ms);
    bool brake(int ms);
    bool shiftdirection();
    bool loadall();
    bool unloadall();
    State forwardstate();
    State backwardstate();
    bool split(int where, Route* assignedroute=nullptr);
    void couple(Train& train, bool ismyback, bool ishisback);
    Tracksystem* tracksystem;
    bool selected = false;
    float speed;
    bool gasisforward = true;
    std::vector<Wagon*> wagons;
    Route* route = nullptr;
    int orderid = 0;
    bool go = false;
    bool wantstocouple = false;
private:
    bool checkifreachedstate(State goalstate, int ms);
    SDL_Texture* lighttex;
    int lightw, lighth;
};

class Building
{
public:
    Building(ResourceManager& resources, int x, int y, int w, int h, resourcetype need, resourcetype production);
    virtual void render();
    void update(int ms);
protected:
    SDL_Color color;
private:
    Storage* storage;
    SDL_Rect rect;
    int timeleft = 3000;
    ResourceManager* allresources;
    resourcetype wants;
    resourcetype makes;
};

class Brewery : public Building
{
public:
    Brewery(ResourceManager& resources, Vec pos);
};

class Hopsfield : public Building
{
public:
    Hopsfield(ResourceManager& resources, Vec pos);
};

class City : public Building
{
public:
    City(ResourceManager& resources, Vec pos);
};

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
