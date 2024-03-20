#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include "bahnhof/common/math.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

int init();

SDL_Texture* loadimage(std::string path);
SDL_Texture* loadtext(std::string text, SDL_Color color);

void close();

class Rendering;

class Sprite
{
public:
    Sprite(std::string pathtopng, int nimages=1, int ntypes=1);
    ~Sprite();
    void render(Rendering* r, Vec pos, bool ported, bool zoomed);
    void updateframe(int ms);
    float imageangle = 0;
    float imagespeed = 0;
    float imageindex = 0;
    float imagetype = 0;
private:
    int w;
    int h;
    SDL_Texture* tex;
    SDL_Rect srcrect;
    SDL_Rect rect;
    int imagenumber;
    int imagetypes;
};