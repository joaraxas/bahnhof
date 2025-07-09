#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<map>
#include<string>
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/spritenames.h"

class Game;
class Rendering;

class Spritesheet
{
public:
    Spritesheet(sprites::name newname, std::string pathtopng, int nimages=1, int ntypes=1);
    ~Spritesheet();
    void render(Rendering* r, Vec pos, bool ported, bool zoomed, float imageangle=0, 
                int imageindex=0, int imagetype=0, float imagescale=1, Uint8 imagealpha=255, 
                Uint8 red=255, Uint8 green=255, Uint8 blue=255);
    int getimagenumber();
    int getimagetypes();
    void setoriginx(int);
    void setoriginy(int);
    Vec getsize();
    sprites::name getname();
private:
    sprites::name name;
    int w;
    int h;
    SDL_Texture* tex;
    SDL_Rect srcrect;
    SDL_Rect rect;
    int imagenumber;
    int imagetypes;
    int origin_x;
    int origin_y;
    Uint8 alpha = 255;
    Uint8 rgb[3] = {255,255,255};
};

class SpriteManager
{
public:
    SpriteManager(Game* _game);
    Spritesheet* get(sprites::name);
    Game* getgame() {return game;};
private:
    Game* game;
    std::map<sprites::name, std::unique_ptr<Spritesheet>> spritemap;
    void addspritesheet(sprites::name, std::string path, int imagenumber=1, int imagetypes=1);
};

class Sprite
{
public:
    void setspritesheet(SpriteManager& s, sprites::name name);
    void updateframe(int ms);
    virtual void render(Rendering* r, Vec pos);
    Vec getsize();
    sprites::name getname();
    float imageangle = 0;
    float imagespeed = 0;
    float imageindex = 0;
    float imagetype = 0;
    float imagescale = 1;
    bool ported = true;
    bool zoomed = true;
    SDL_Color color = {255,255,255,255};
private:
    Spritesheet* spritesheet = nullptr;
    int imagenumber = 0;
    int imagetypes = 0;
};

class Icon : public Sprite
{
public:
    Icon();
    void setspritesheet(SpriteManager& s, sprites::name name);
    void render(Rendering* r, Vec pos);
};