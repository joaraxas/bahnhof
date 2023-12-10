extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern const Uint8* keys;
const int gasbutton = SDL_SCANCODE_W;
const int breakbutton = SDL_SCANCODE_S;

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

class Train
{
    public:
    Train(Tracksystem* newtracksystem);
    void getinput();
    void update(int ms);
    void render();
    Tracksystem* tracksystem;
    private:
    Track* track;
    float nodedist;
    Vec pos;
    float speed;
    bool selected = true;
    bool alignedwithtrackdirection = true;
    int w;
    int h;
    float imageangle = 0;
    SDL_Texture* tex;
};