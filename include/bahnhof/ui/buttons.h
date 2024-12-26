#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"
#include "bahnhof/ui/tables.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{
class Panel;

class Button : public Element
{
public:
    Button(Panel*, Vec newpos);
    virtual ~Button() {std::cout<<"del button"<<std::endl;};
    bool checkclick(Vec pos, int type);
    virtual void render(Rendering*);
protected:
    virtual void click() {};
};

class TextButton : public Button
{
public:
    TextButton(Panel*, Vec newpos, std::string text);
    virtual ~TextButton() {std::cout<<"del textbutton"<<std::endl;};
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
    void click();
};

class PlaceSignal : public TextButton
{
public:
    PlaceSignal(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build signal") {}
protected:
    void click();
};

class PlaceTrack : public TextButton
{
public:
    PlaceTrack(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build track") {}
protected:
    void click();
};

class BuildWagon : public TextButton
{
public:
    BuildWagon(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build signal") {}
protected:
    void click();
};

class ManageRoutes : public TextButton
{
public:
    ManageRoutes(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Manage routes") {}
protected:
    void click();
};

class ManageTrains : public TextButton
{
public:
    ManageTrains(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Manage trains") {}
protected:
    void click();
};

}