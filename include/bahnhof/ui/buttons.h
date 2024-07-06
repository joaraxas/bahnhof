#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{
class Panel;

class Button
{
public:
    Button(Panel*, Vec newpos);
    bool checkclick(Vec pos, int type);
    virtual void render(Rendering*);
    SDL_Rect getglobalrect();
    SDL_Rect getlocalrect();
protected:
    virtual void click() {};
    Panel* panel;
    SDL_Rect rect;
    InterfaceManager* ui;
    Game* game;
};

class TextButton : public Button
{
public:
    TextButton(Panel*, Vec newpos, std::string text);
    virtual void render(Rendering*);
private:
    int textheight;
    int textwidth;
    int maxtextwidth;
    std::string text;
};

class Close : public TextButton
{
public:
    Close(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Close") {}
protected:
    virtual void click();
};

class PlaceSignal : public TextButton
{
public:
    PlaceSignal(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build signal") {}
protected:
    virtual void click();
};

class PlaceTrack : public TextButton
{
public:
    PlaceTrack(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build track") {}
protected:
    virtual void click();
};

class BuildWagon : public TextButton
{
public:
    BuildWagon(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build signal") {}
protected:
    virtual void click();
};

}