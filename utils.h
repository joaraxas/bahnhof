extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern const Uint8* keys;
const int gasbutton = SDL_SCANCODE_W;
const int breakbutton = SDL_SCANCODE_S;
const int numberbuttons[10] = {SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9};

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const double pi = 3.141592653589793238;

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

class Tracksystem
{
public:
    Tracksystem(std::vector<float> xs, std::vector<float> ys);
    void render();
    void leftclick(int xMouse, int yMouse);
    void rightclick(int xMouse, int yMouse);
    void addnode(float x, float y, Node* previousnode);
    void addtrack(Node* leftnode, Node* rightnode, int ind);
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Track>> tracks;
    Node* selectednode = nullptr;
};

class Node
{
public:
    Node(float xstart, float ystart, float dirstart);
    Vec pos;
    float dir;
    std::vector<Track*> tracksleft;
    std::vector<Track*> tracksright;
    int stateleft;
    int stateright;
};

class Track
{
public:
    Track(Node* left, Node* right, int ind);
    ~Track();
    int indexx;
    Node* nodeleft;
    Node* noderight;
    float radius;
    float phi;
    float y0;
    float getradius();
    void render();
    Vec getpos(float nodedist);
    float getarclength(float nodedist);
};

class Wagon
{
public:
    Wagon(Tracksystem* newtracksystem, float nodediststart);
    void update(int ms);
    void render();
    Tracksystem* tracksystem;
    float nodedist;
    Train* train;
    Vec pos;
    bool alignedwithtrackdirection = true;
private:
    Track* track;
    int w;
    int h;
    float imageangle = 0;
    SDL_Texture* tex;
};

class Train
{
public:
    Train(Tracksystem* newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed);
    void getinput();
    //void update(int ms);
    Tracksystem* tracksystem;
    bool selected = false;
    float speed;
    std::vector<Wagon*> wagons;
    void checkCollision(Train* train);
    void split(int where);
    void couple(Train& train, bool ismyback, bool ishisback);
};

extern std::vector<std::unique_ptr<Train> > trains;
extern std::vector<std::unique_ptr<Wagon> > wagons;