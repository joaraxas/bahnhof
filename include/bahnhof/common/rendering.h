#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include "math.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const int MAP_WIDTH = SCREEN_WIDTH*64;
const int MAP_HEIGHT = SCREEN_HEIGHT*64;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

extern float money;
extern bool nicetracks;

int init();

SDL_Texture* loadImage(std::string path);
SDL_Texture* loadText(std::string text, SDL_Color color);

void close();

class Game; class Camera;

class Rendering
{
public:
    Rendering(Game* whatgame, Camera* whatcam);
    void render();
    void rendertext(std::string text, int x, int y, SDL_Color color={0,0,0,255}, bool ported=true, bool zoomed=false);
    void rendertexture(SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect=nullptr, float angle=0, bool ported=true, bool zoomed=true);
    void renderline(Vec pos1, Vec pos2, bool ported=true);
    void renderrectangle(SDL_Rect* rect, bool ported=true, bool zoomed=true);
    void renderfilledrectangle(SDL_Rect* rect, bool ported=true, bool zoomed=true);
    float getscale();
private:
    Game* game;
    Camera* cam;
    SDL_Texture* fieldtex;
};

Vec randpos(int xoffset=0, int yoffset=0);