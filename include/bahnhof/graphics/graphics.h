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

int setfontsize(int size);
SDL_Texture* loadimage(std::string path);
SDL_Texture* loadtext(std::string text, SDL_Color color, int maxwidth=0);

void close();