#pragma once
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/graphics/sprite.h"


class InputManager;
class RollingStockManager;
class Train;
class Wagon;

namespace Tracks{
    class Tracksystem;
}

struct WagonInfo
{
    WagonInfo(sprites::name a, resourcetype b, int c) : iconname(a), loadedresource(b), loadamount(c) {};
    sprites::name iconname;
    resourcetype loadedresource;
    int loadamount;
};

struct TrainInfo
{
    TrainInfo(Train* a, std::string b, float c, std::vector<WagonInfo> d) : train(a), name(b), speed(c), wagoninfos(d) {};
    Train* train;
    std::string name;
    float speed;
    std::vector<WagonInfo> wagoninfos;
};

class TrainManager
{
public:
    TrainManager(Tracks::Tracksystem* newtracks, RollingStockManager& r);
    void update(int ms);
    void getinput(InputManager* input, int mslogic); 
    void render(Rendering* r);
    void addwagon(Wagon* wagon);
    void addtrain(Train* train);
    void deselectall();
    Train* gettrainatpos(Vec pos);
    std::vector<TrainInfo> gettrainsinfo();
    void inittrain(State startstate);
    void addtrainstoorphans(float speed=0);
private:
    RollingStockManager& rollingstock;
    std::string generatetrainname();
    std::vector<std::unique_ptr<Wagon>> wagons;
    std::vector<std::unique_ptr<Train>> trains;
    Tracks::Tracksystem* tracks = nullptr;
};