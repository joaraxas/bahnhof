#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/track/state.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/ui/ownership.h"

namespace Tracks{
    class Tracksystem;
}
struct Order;

class Route
{
public:
    Route(Tracks::Tracksystem* tracks, std::string routename);
    Order* getorder(int orderid) const;
    int nextorder(int orderid);
    int previousorder(int orderid);
    int appendorder(Order* order);
    int insertorderatselected(Order* order);
    int insertorder(Order* order, int orderid);
    void removeselectedorder();
    void removeorder(int orderid);
    void removeordersupto(int orderid);
    void removeorders(int orderindexfrom, int orderindexto);
    void render(Rendering* r);
    int getindex(int orderid) const;
    std::vector<std::string> getorderdescriptions();
    std::vector<int> getorderids();
    std::vector<int> getordernumberstorender();
    std::string name = "New route";
    int selectedorderid = -1;
    std::vector<signalid> signals;
    std::vector<switchid> switches;
private:
    Tracks::Tracksystem* tracks;
    std::vector<std::unique_ptr<Order>> orders;
    int ordercounter = 0;
    std::vector<int> orderids;
};

class RouteManager
{
public:
    RouteManager(Tracks::Tracksystem* tracks);
    void addroute();
    Route* getroute(int id);
    std::vector<std::string> getroutenames();
    std::vector<int> getrouteids();
    int getnumberofroutes();
    void createlistpanel();
private:
    Tracks::Tracksystem* tracksystem;
    std::vector<std::unique_ptr<Route>> routes;
    UI::Ownership routelistpanel;
};

enum ordertype
{
    gotostate, o_setsignal, o_setswitch, o_couple, decouple, turn, loadresource, wipe
};
struct Order
{
    virtual ~Order() {};//std::cout<<"del order"<<std::endl;};
    virtual void assignroute(Route* newroute, Tracks::Tracksystem* tracksystem);
    virtual void render(Rendering* r, int number) {};
    void renderlabel(Rendering* r, Vec pos, int number, SDL_Color bgrcol={255,255,255,255}, SDL_Color textcol = {0,0,0,255});
    void invalidate();
    Route* route = nullptr; // initialized by route
    Tracks::Tracksystem* tracks = nullptr; // initialized by route
    ordertype order;
    std::string description;
    int offset = 0;
    bool valid = true;
};
struct Gotostate : public Order
{
    Gotostate(State whichstate, bool mustpass=false);
    ~Gotostate();
    void assignroute(Route* newroute, Tracks::Tracksystem* tracksystem);
    void render(Rendering* r, int number);
    State state;
    bool pass;
	Vec posleft;
	Vec posright;
};
struct Setsignal : public Order
{
    Setsignal(signalid whichsignal, int redgreenorflip=2);
    ~Setsignal();
    void assignroute(Route* newroute, Tracks::Tracksystem* tracksystem);
    void render(Rendering* r, int number);
    signalid signal;
    int redgreenflip;
	Vec pos;
};
struct Setswitch : public Order
{
    Setswitch(switchid whichswitch, int whichnodestate=-1);
    ~Setswitch();
    void assignroute(Route* newroute, Tracks::Tracksystem* tracksystem);
    void render(Rendering* r, int number);
    switchid _switch;
    bool flip;
    int switchstate;
	Vec pos;
};
struct Couple : public Order
{
    Couple();
};
struct Decouple : public Order
{
    Decouple(Route* givewhatroute) : Decouple(1, givewhatroute) {};
    Decouple(int keephowmany=1, Route* givewhatroute=nullptr);
    int where;
    Route* route;
};
struct Turn : public Order
{
    Turn();
    ~Turn() {};//std::cout<<"del order"<<std::endl;};std::cout<<"del turn"<<std::endl;};
};
struct Loadresource : public Order
{
    Loadresource();
    ~Loadresource() {};//std::cout<<"del order"<<std::endl;};std::cout<<"del loadres"<<std::endl;};
    Loadresource(int loadunloadorboth);
    Loadresource(resourcetype whichresource, int loadunloadorboth);
    resourcetype resource;
    bool anyresource;
    bool loading;
    bool unloading;
};
struct Wipe : public Order
{
    Wipe();
    ~Wipe() {};//std::cout<<"del order"<<std::endl;};std::cout<<"del wipe"<<std::endl;};
};
