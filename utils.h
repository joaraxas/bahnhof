extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

extern const Uint8* keys;
const int gasbutton = SDL_SCANCODE_W;
const int brakebutton = SDL_SCANCODE_S;
const int gearbutton = SDL_SCANCODE_LSHIFT;
const int numberbuttons[10] = {SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9};
const int loadbutton = SDL_SCANCODE_L;
const int unloadbutton = SDL_SCANCODE_U;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const double pi = 3.141592653589793238;

extern float money;
extern bool nicetracks;
extern float scale;
extern int xMouse, yMouse;

int init();

SDL_Texture* loadImage(std::string path);
SDL_Texture* loadText(std::string text, SDL_Color color);
void rendertext(std::string text, int x, int y, SDL_Color color);

void close();

class Vec
{
    public:
    Vec();
    Vec(float xstart, float ystart);
    Vec operator+(Vec);
    Vec operator-(Vec);
    Vec operator*(float);
    Vec operator/(float);
    float x;
    float y;
};

float norm(Vec v);
float sign(float a);
float truncate(float dir);

class Node;
class Track;
class Wagon;
class Train;
class Route;
struct Order;
class ResourceManager;
class Resource;
class Storage;
class Signal;

enum resourcetype
{
    none=-1, beer, hops, barley
};

typedef int trackid;
typedef int nodeid;
typedef int signalid;

struct State
{
    State();
    State(trackid trackstart, float nodediststart, bool alignedwithtrack);
    trackid track;
    float nodedist;
    bool alignedwithtrack;
};

class Tracksystem
{
friend class Node;
friend class Track;
friend class Gamestate;
public:
    Tracksystem();
    Tracksystem(ResourceManager& resources, std::vector<float> xs, std::vector<float> ys);
    ~Tracksystem();
    void render();
    void deleteclick(int xMouse, int yMouse);
    void selectat(Vec pos);
    void buildat(Vec pos);
    bool switchat(Vec pos);
    Order* generateorderat(Vec pos);
    Vec getpos(State state, float transverseoffset=0);
    Vec getsignalpos(signalid signal);
    Vec getswitchpos(nodeid node, bool updown);
    State getcloseststate(Vec pos);
    float getorientation(State state);
    State travel(State state, float pixels);
    ResourceManager* allresources;
    signalid addsignal(State state);
    bool setsignal(signalid signal, int redgreenorflip);
    bool isred(State trainstate, float pixels);
    int setswitch(nodeid node, bool updown, int switchstate);
    nodeid selectednode = 0;
    bool placingsignal = false;
private:
    nodeid addnode(Vec pos, float dir);
    trackid addtrack(nodeid leftnode, nodeid rightnode);
    void removenode(nodeid toremove);
    void removetrack(trackid toremove);
    void removesignal(signalid toremove);
    State whatdidiclick(Vec mousepos, trackid* track, nodeid* node, signalid* signal, nodeid* _switch);
    float distancetotrack(trackid track, Vec pos);
    float distancetonode(nodeid node, Vec pos);
    float distancetosignal(signalid node, Vec pos);
    float distancetoswitch(nodeid node, Vec pos, bool updown);
    nodeid getclosestnode(Vec pos);
    signalid getclosestsignal(Vec pos);
    nodeid getclosestswitch(Vec pos, bool* updown);
    Node* getnode(nodeid node);
    float getnodedir(nodeid node);
    Vec getnodepos(nodeid node);
    Track* gettrack(trackid track);
    trackid nexttrack(trackid track);
    trackid previoustrack(trackid track);
    nodeid extendtracktopos(nodeid fromnode, Vec pos);
    void connecttwonodes(nodeid node1, nodeid node2);
    Signal* getsignal(signalid signal);
    std::map<nodeid, Node*> nodes;
    std::map<trackid, Track*> tracks;
    std::map<signalid, Signal*> signals;
    nodeid nodecounter = 0;
    trackid trackcounter = 0;
    signalid signalcounter = 0;
    bool preparingtrack = false;
};

class Node
{
friend class Tracksystem;
private:
    Node(Tracksystem& newtracksystem, Vec posstart, float dirstart, nodeid myid);
    void render();
    trackid gettrackup();
    trackid gettrackdown();
    Vec getswitchpos(bool updown);
    void incrementswitch(bool updown);
    Tracksystem* tracksystem;
    nodeid id;
    Vec pos;
    float dir;
    int stateup = 0;
    int statedown = 0;
    std::vector<trackid> tracksup;
    std::vector<trackid> tracksdown;
};

class Track
{
friend class Tracksystem;
private:
    Track(Tracksystem& newtracksystem, nodeid previous, nodeid next, trackid myid);
    ~Track();
    Tracksystem* tracksystem;
    nodeid previousnode, nextnode;
    void render();
    Vec getpos(float nodedist);
    Vec getpos(float nodedist, float transverseoffset);
    State getcloseststate(Vec pos);
    float getarclength(float nodedist);
    float getorientation(float nodedist);
    bool isabovepreviousnode();
    bool isbelownextnode();
    trackid id;
    float phi;
    float radius;
    float previousdir;
    float nextdir;
    Vec previouspos;
    Vec nextpos;
};

class Signal
{
friend class Tracksystem;
public:
    Signal(Tracksystem& newtracksystem, State signalstate);
    void render();
    bool isred(State trainstate, float pixels);
    bool isgreen = true;
private:
    State state;
    Vec pos;
    Tracksystem* tracksystem;
};

class Wagon
{
public:
    Wagon(Tracksystem& newtracksystem, State trackstate, std::string path);
    void travel(float pixels);
    virtual void update(int ms);
    virtual void render();
    virtual Vec getpos(bool front);
    virtual int loadwagon(resourcetype type, int amount);
    virtual int unloadwagon(resourcetype* type);
    virtual float getpower();
    Train* train;
    Vec pos;
    bool alignedforward = true;
    int w;
    State state; //should be protected
protected:
    Tracksystem* tracksystem;
    int h;
    float imageangle = 0;
    SDL_Texture* tex;
    ResourceManager* allresources;
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
    float maxspeed[2] = {4*40,140};
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
    bool checkifreachedstate(State goalstate);
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
};

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
    gotostate, o_setsignal, o_setswitch, couple, decouple, turn, loadresource, wipe
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

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();
    Resource* get(resourcetype type);
private:
    std::map<resourcetype, Resource*> resourcemap;
};

class Resource
{
public:
    Resource(resourcetype newtype, std::string newname, std::string pathtotex);
    void render(Vec pos);
    resourcetype type;
private:
    std::string name;
    SDL_Texture* tex;
    int w;
    int h;
};

class Storage
{
public:
    Storage(ResourceManager& resources, int x, int y, int w, int h, resourcetype _accepts, resourcetype _provides);
    void render();
    int loadstorage(resourcetype type, int amount);
    int unloadstorage(resourcetype type, int amount);
    bool containspoint(Vec pos);
    resourcetype getfirststoredresource();
    resourcetype accepts;
    resourcetype provides;
private:
    ResourceManager* allresources;
    std::map<resourcetype, int> storedresources;
    SDL_Rect rect;
};

Storage* getstorageatpoint(Vec pos);

class Building
{
public:
    Building(ResourceManager& resources, int x, int y, int w, int h, resourcetype need, resourcetype production);
    virtual void render();
    void update(int ms);
    bool containspoint(Vec pos);
    SDL_Rect rect;
private:
    Storage* storage;
    int timeleft = 3000;
    ResourceManager* allresources;
    resourcetype wants;
    resourcetype makes;
};

class Brewery : public Building
{
public:
    Brewery(ResourceManager& resources, int x, int y, int w, int h);
    void render();
};

class Hopsfield : public Building
{
public:
    Hopsfield(ResourceManager& resources, int x, int y, int w, int h);
};

class City : public Building
{
public:
    City(ResourceManager& resources, int x, int y, int w, int h);
};

class Gamestate
{
public:
    Gamestate();
    ~Gamestate();
    void initthreetrains();
    void initcoupling();
    void initjusttrack();
    void inittrain(State startstate);
	ResourceManager resources;
    std::vector<Wagon*> wagons;
    std::vector<std::unique_ptr<Route>> routes;
    std::unique_ptr<Tracksystem> tracksystem;
    Route* selectedroute = nullptr;
};

extern std::vector<std::unique_ptr<Train> > trains;
extern std::vector<std::unique_ptr<Storage> > storages;
extern std::vector<std::unique_ptr<Building> > buildings;