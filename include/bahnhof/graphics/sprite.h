#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<map>
#include<string>
#include "bahnhof/common/math.h"

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
    switchsprite,
    signal
};
}

class Rendering;

class Spritesheet
{
public:
    Spritesheet(sprites::name newname, std::string pathtopng, int nimages=1, int ntypes=1);
    ~Spritesheet();
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
    Spritesheet* get(sprites::name);
private:
    std::map<sprites::name, std::unique_ptr<Spritesheet>> spritemap;
    void addspritesheet(sprites::name, std::string path, int imagenumber=1, int imagetypes=1);
};

class Sprite
{
public:
    void setspritesheet(SpriteManager* s, sprites::name name);
    void updateframe(int ms);
    void render(Rendering* r, Vec pos);
    float imageangle = 0;
    float imagespeed = 0;
    float imageindex = 0;
    float imagetype = 0;
    bool ported = true;
    bool zoomed = true;
private:
    Spritesheet* spritesheet = nullptr;
    int imagenumber = 0;
    int imagetypes = 0;
};
