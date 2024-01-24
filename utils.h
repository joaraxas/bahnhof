extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern const Uint8* keys;
const int gasbutton = SDL_SCANCODE_W;
const int breakbutton = SDL_SCANCODE_S;
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

enum resourcetype
{
    none=-1, beer, hops, barley
};

class Tracksystem
{
public:
    Tracksystem(ResourceManager& resources, std::vector<float> xs, std::vector<float> ys);
    void render();
    void leftclick(int xMouse, int yMouse);
    void rightclick(int xMouse, int yMouse);
    Node* addnode(float x, float y, float dir);
    Track* addtrack(Node* leftnode, Node* rightnode);
    ResourceManager* allresources;
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Track>> tracks;
private:
    Node* getclosestnode(Vec pos);
    Node* selectednode = nullptr;
    Node* extendtrackto(Node* fromnode, Vec pos);
    void connecttwonodes(Node* node1, Node* node2);
};

class Node
{
public:
    Node(float xstart, float ystart, float dirstart);
    Vec pos;
    float dir;
    void render();
    Track* getrighttrack();
    Track* getlefttrack();
    void incrementswitch();
    std::vector<Track*> tracksleft;
    std::vector<Track*> tracksright;
private:
    int stateleft;
    int stateright;
};

class Track
{
public:
    Track(Node* left, Node* right);
    ~Track();
    Node* nodeleft;
    Node* noderight;
    void render();
    Vec getpos(float nodedist);
    Vec getpos(float nodedist, float transverseoffset);
    float getarclength(float nodedist);
    float getorientation(float nodedist);
    bool isrightofleftnode();
    bool isleftofrightnode();
    Track* getrighttrack();
    Track* getlefttrack();
private:
    float phi;
    float radius;
};

class Wagon
{
public:
    Wagon(Tracksystem& newtracksystem, float nodediststart, std::string path);
    virtual void update(int ms);
    virtual void render();
    virtual int loadwagon(resourcetype type, int amount);
    virtual int unloadwagon(resourcetype* type);
    virtual float getpower();
    Tracksystem* tracksystem;
    float nodedist;
    Train* train;
    Vec pos;
    bool alignedwithtrackdirection = true;
    bool alignedforward = true;
    int w;
protected:
    Track* track;
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
    Locomotive(Tracksystem& newtracksystem, float nodediststart);
    void render();
    void update(int ms);
    int loadwagon(resourcetype type, int amount);
    int unloadwagon(resourcetype* type);
    float getpower();
private:
    float P[2] = {0.2*4,0.2*4};
    float maxspeed[2] = {40*4,4*140};
    int imagenumber = 4;
    float imageindex = 0;
    float imagespeed = 2;
};

class Openwagon : public Wagon
{
public:
    Openwagon(Tracksystem& newtracksystem, float nodediststart);
    int loadwagon(resourcetype type, int amount);
};

class Tankwagon : public Wagon
{
public:
    Tankwagon(Tracksystem& newtracksystem, float nodediststart);
    int loadwagon(resourcetype type, int amount);
};

class Train
{
public:
    Train(Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed);
    void getinput(int ms);
    void checkCollision(Train* train);
    void split(int where);
    void couple(Train& train, bool ismyback, bool ishisback);
    Tracksystem* tracksystem;
    bool selected = false;
    float speed;
    bool direction = 1;
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
    Storage(ResourceManager& resources, int x, int y, int w, int h);
    void render();
    int loadstorage(resourcetype type, int amount);
    int unloadstorage(resourcetype type, int amount);
    bool containspoint(Vec pos);
    resourcetype getfirststoredresource();
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

extern std::vector<std::unique_ptr<Train> > trains;
extern std::vector<std::unique_ptr<Storage> > storages;
extern resourcetype selectedresource;