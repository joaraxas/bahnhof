extern SDL_Window* window;
extern SDL_Renderer* renderer;

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
public:
    Tracksystem(ResourceManager& resources, std::vector<float> xs, std::vector<float> ys);
    ~Tracksystem();
    void render();
    void leftclick(int xMouse, int yMouse);
    void rightclick(int xMouse, int yMouse);
    void deleteclick(int xMouse, int yMouse);
    Vec getpos(State state);
    float getorientation(State state);
    State travel(State state, float pixels);
    ResourceManager* allresources;
    signalid addsignal(State state);
    bool isred(State trainstate, float pixels);
private:
    nodeid addnode(Vec pos, float dir);
    trackid addtrack(nodeid leftnode, nodeid rightnode);
    void removenode(nodeid nodetoremove);
    void removetrack(trackid tracktoremove);
    float distancetonode(nodeid node, Vec pos);
    nodeid getclosestnode(Vec pos);
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
    nodeid selectednode = 0;
    nodeid nodecounter = 0;
    trackid trackcounter = 0;
    signalid signalcounter = 0;
    bool preparingtrack = false;
};

class Node
{
friend class Tracksystem;
private:
    Node(Tracksystem& newtracksystem, Vec posstart, float dirstart);
    void render();
    trackid gettrackup();
    trackid gettrackdown();
    void incrementswitch();
    Tracksystem* tracksystem;
    Vec pos;
    float dir;
    int stateup;
    int statedown;
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
    void checkCollision(int ms, Train* train);
    void gas(int ms);
    void brake(int ms);
    void split(int where);
    void couple(Train& train, bool ismyback, bool ishisback);
    Tracksystem* tracksystem;
    bool selected = false;
    float speed;
    bool gasisforward = true;
    std::vector<Wagon*> wagons;
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
    int timeleft;
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

class Signal
{
public:
    Signal(Tracksystem& newtracksystem, State signalstate);
    void render();
    bool isred(State trainstate, float pixels);
    bool isgreen = false;
private:
    State state;
    Vec pos;
    Tracksystem* tracksystem;
};

extern std::vector<std::unique_ptr<Train> > trains;
extern std::vector<std::unique_ptr<Storage> > storages;