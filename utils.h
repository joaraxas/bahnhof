extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern const Uint8* keys;

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
    float x;
    float y;
};

float sign(float a);

class Node;

class Tracksystem
{
    public:
    Tracksystem(std::vector<float> xs, std::vector<float> ys);
    void render();
    void addnode(float x, float y, Node previousnode);
    float getradius(Node node1, Node node2);
    Vec getpos(Node node1, Node node2, float nodedist);
    float getarclength(Node node1, Node node2, float nodedist);
    std::vector<Node> nodes;
};

class Node
{
    public:
    Node(float xstart, float ystart, float dirstart);
    Tracksystem* tracksystem;
    Vec pos;
    float dir;
};

class Train
{
    public:
    Train(Tracksystem* newtracksystem);
    void render();
    void update(int ms);
    Tracksystem* tracksystem;
    private:
    int nodepair[2];
    float nodedist;
    Vec pos;
    float speed;
    int w;
    int h;
    float imageangle = 0;
    SDL_Texture* tex;
};