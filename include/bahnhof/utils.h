#ifndef UTILS_H
#define UTILS_H

//#include "bahnhof/resources/resources.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Rect cam;
extern TTF_Font* font;

extern float money;
extern bool nicetracks;
extern float scale;
extern int xMouse, yMouse;

int init();

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

SDL_Texture* loadImage(std::string path);
SDL_Texture* loadText(std::string text, SDL_Color color);
void rendertext(std::string text, int x, int y, SDL_Color color={0,0,0,255}, bool ported=true, bool zoomed=false);
void rendertexture(SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect=nullptr, float angle=0, bool ported=true, bool zoomed=true);
void renderline(Vec pos1, Vec pos2, bool ported=true);
void renderrectangle(SDL_Rect* rect, bool ported=true, bool zoomed=true);
void renderfilledrectangle(SDL_Rect* rect, bool ported=true, bool zoomed=true);

void close();

float norm(Vec v);
float sign(float a);
float truncate(float dir);
int randint(int maxinclusive);

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

State flipstate(State state);

#endif