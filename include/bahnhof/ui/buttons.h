#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<vector>
#include "bahnhof/common/math.h"
#include "bahnhof/ui/ui.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{

class Button : virtual public Element
{
public:
    Button(Panel*, Vec newpos);
    virtual ~Button() {};
    bool checkclick(Vec pos, int type);
    virtual void render(Rendering*);
protected:
    virtual void click() {};
};

class TextButton : public Button
{
public:
    TextButton(Panel*, Vec newpos, std::string text, int width=80);
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
    Close(Panel* newpanel, Vec newpos) : Element(newpanel), TextButton(newpanel, newpos, "Close") {}
protected:
    void click();
};

class PlaceSignal : public TextButton
{
public:
    PlaceSignal(Panel* newpanel, Vec newpos) : Element(newpanel), TextButton(newpanel, newpos, "Build signal") {}
protected:
    void click();
};

class PlaceTrack : public TextButton
{
public:
    PlaceTrack(Panel* newpanel, Vec newpos) : Element(newpanel), TextButton(newpanel, newpos, "Build track") {}
protected:
    void click();
};

class BuildWagon : public TextButton
{
public:
    BuildWagon(Panel* newpanel, Vec newpos) : Element(newpanel), TextButton(newpanel, newpos, "Build signal") {}
protected:
    void click();
};

class ManageRoutes : public TextButton
{
public:
    ManageRoutes(Panel* newpanel, Vec newpos) : Element(newpanel), TextButton(newpanel, newpos, "Manage routes") {}
protected:
    void click();
};

class ManageTrains : public TextButton
{
public:
    ManageTrains(Panel* newpanel, Vec newpos) : Element(newpanel), TextButton(newpanel, newpos, "Manage trains") {}
protected:
    void click();
};

namespace Routing
{

class AddOrder : public TextButton
{
public:
    AddOrder(Panel* newpanel, Vec newpos, Route* whatroute, std::string text) : 
                            Element(newpanel), 
                            TextButton(newpanel, newpos, text, 120),
                            route(whatroute) {}
protected:
    Route* route;
};

class AddTurn : public AddOrder
{
public:
    AddTurn(Panel* newpanel, Vec newpos, Route* whatroute) : Element(newpanel), AddOrder(newpanel, newpos, whatroute, "Reverse direction") {}
protected:
    void click();
};

class AddCouple : public AddOrder
{
public:
    AddCouple(Panel* newpanel, Vec newpos, Route* whatroute) : Element(newpanel), AddOrder(newpanel, newpos, whatroute, "Couple") {}
protected:
    void click();
};

class AddDecouple : public AddOrder
{
public:
    AddDecouple(Panel* newpanel, Vec newpos, Route* whatroute) : Element(newpanel), AddOrder(newpanel, newpos, whatroute, "Decouple") {}
protected:
    void click();
};

class AddLoadResource : public AddOrder
{
public:
    AddLoadResource(Panel* newpanel, Vec newpos, Route* whatroute) : Element(newpanel), AddOrder(newpanel, newpos, whatroute, "Load resource") {}
protected:
    void click();
};

class RemoveOrder : public AddOrder
{
public:
    RemoveOrder(Panel* newpanel, Vec newpos, Route* whatroute) : Element(newpanel), AddOrder(newpanel, newpos, whatroute, "Remove selected") {}
protected:
    void click();
};

} //namespace Routing
} //namespace UI