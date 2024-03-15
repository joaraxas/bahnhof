class Tracksystem;

class Route
{
public:
    Route(Tracksystem* whattracksystem, std::string routename);
    Order* getorder(int orderid);
    int nextorder(int orderid);
    int previousorder(int orderid);
    int appendorder(Order* order);
    int insertorderatselected(Order* order);
    int insertorder(Order* order, int orderid);
    void removeselectedorder();
    void removeorder(int orderid);
    void removeordersupto(int orderid);
    void removeorders(int orderindexfrom, int orderindexto);
    void render();
    int getindex(int orderid);
    std::string name = "New route";
    int selectedorderid = -1;
    Tracksystem* tracksystem;
    std::vector<signalid> signals;
    std::vector<nodeid> switches;
    std::vector<bool> updowns;
private:
    std::vector<std::unique_ptr<Order>> orders;
    int ordercounter = 0;
    std::vector<int> orderids;
};

enum ordertype
{
    gotostate, o_setsignal, o_setswitch, o_couple, decouple, turn, loadresource, wipe
};
struct Order
{
    virtual ~Order() {};//std::cout<<"del order"<<std::endl;};
    virtual void assignroute(Route* newroute);
    virtual void render(int number) {};
    void renderlabel(Vec pos, int number, SDL_Color bgrcol={255,255,255,255}, SDL_Color textcol = {0,0,0,255});
    Route* route = nullptr; // initialized by route
    ordertype order;
    std::string description;
    int offset = 0;
};
struct Gotostate : public Order
{
    Gotostate(State whichstate, bool mustpass=false);
    void render(int number);
    State state;
    bool pass;
};
struct Setsignal : public Order
{
    Setsignal(signalid whichsignal, int redgreenorflip=2);
    void assignroute(Route* newroute);
    void render(int number);
    signalid signal;
    int redgreenflip;
};
struct Setswitch : public Order
{
    Setswitch(nodeid whichnode, bool ispointingup, int whichnodestate=-1);
    void assignroute(Route* newroute);
    void render(int number);
    nodeid node;
    bool updown;
    bool flip;
    int nodestate;
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
