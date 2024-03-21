#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "bahnhof/common/math.h"

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

int init();

SDL_Texture* loadimage(std::string path);
SDL_Texture* loadtext(std::string text, SDL_Color color);

void close();

class Rendering;

namespace sprites{
enum name
{
    none=-1,
    openwagon,
    refrigeratorcar,
    tankloco,
    iconopenwagon,
    iconrefrigeratorcar,
    icontankloco,
    signal
};
}

class Sprite
{
public:
    Sprite(sprites::name newname, std::string pathtopng, int nimages=1, int ntypes=1);
    ~Sprite();
    void render(Rendering* r, Vec pos, bool ported, bool zoomed, float imageangle=0, int imageindex=0, int imagetype=0);
    int getimagenumber();
    int getimagetypes();
private:
    sprites::name name;
    int w;
    int h;
    SDL_Texture* tex;
    SDL_Rect srcrect;
    SDL_Rect rect;
    int imagenumber;
    int imagetypes;
};

class SpriteManager
{
public:
    SpriteManager();
    Sprite* get(sprites::name);
private:
    std::map<sprites::name, std::unique_ptr<Sprite>> spritemap;
};

class Animation
{
public:
    void setsprite(SpriteManager* s, sprites::name name);
    void updateframe(int ms);
    void render(Rendering* r, Vec pos);
    float imageangle = 0;
    float imagespeed = 0;
    float imageindex = 0;
    float imagetype = 0;
    bool ported = true;
    bool zoomed = true;
private:
    Sprite* sprite = nullptr;
    int imagenumber = 0;
    int imagetypes = 0;
};