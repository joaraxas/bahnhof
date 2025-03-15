#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<iostream>
#include "bahnhof/common/math.h"
#include "bahnhof/ui/ui.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;

namespace UI{

class Button : public Element
{
public:
    Button(Panel*, Vec newpos);
    virtual ~Button() {};
    void mousehover(Vec pos, int ms);
    virtual void render(Rendering*);
private:
    bool highlighted = false;
};

class TextButton : public Button
{
public:
    TextButton(Panel*, Vec newpos, std::string text, int width=80);
    virtual ~TextButton() {std::cout<<"del textbutton"<<std::endl;};
    virtual void render(Rendering*);
protected:
    int maxtextwidth;
    std::string text;
};

class Close : public TextButton
{
public:
    Close(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Close") {};
    void leftclick(Vec mousepos);
};

class PlaceSignal : public TextButton
{
public:
    PlaceSignal(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build signal") {};
    void leftclick(Vec mousepos);
};

class PlaceTrack : public TextButton
{
public:
    PlaceTrack(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Build track") {};
    void leftclick(Vec mousepos);
};

class ManageRoutes : public TextButton
{
public:
    ManageRoutes(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Manage routes") {};
    void leftclick(Vec mousepos);
};

class ManageTrains : public TextButton
{
public:
    ManageTrains(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Manage trains") {};
    void leftclick(Vec mousepos);
};

class IncreaseUIScale : public TextButton
{
public:
    IncreaseUIScale(Panel* p, Vec pos) : TextButton(p, pos, "Increase UI size") {};
    void leftclick(Vec mousepos);
};

class DecreaseUIScale : public TextButton
{
public:
    DecreaseUIScale(Panel* p, Vec pos) : TextButton(p, pos, "Decrease UI size") {};
    void leftclick(Vec mousepos);
};

class SetRoute : public TextButton
{
public:
    SetRoute(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Change route") {};
    void update(int ms);
    void leftclick(Vec mousepos);
};

class GoTrain : public TextButton
{
public:
    GoTrain(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Start route") {};
    void update(int ms);
    void leftclick(Vec mousepos);
};

class GasTrain : public TextButton
{
public:
    GasTrain(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Gas") {};
    void leftpressed(Vec mousepos, int mslogic);
};

class BrakeTrain : public TextButton
{
public:
    BrakeTrain(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Brake") {};
    void leftpressed(Vec mousepos, int mslogic);
};

class TurnTrain : public TextButton
{
public:
    TurnTrain(Panel* newpanel, Vec newpos) : TextButton(newpanel, newpos, "Reverse") {};
    void leftclick(Vec mousepos);
};


namespace Routing
{

class AddOrder : public TextButton
{
public:
    AddOrder(Panel* newpanel, Vec newpos, Route* whatroute, std::string text) : 
                            TextButton(newpanel, newpos, text, 120),
                            route(whatroute) {}
protected:
    Route* route;
};

class AddTurn : public AddOrder
{
public:
    AddTurn(Panel* newpanel, Vec newpos, Route* whatroute) : AddOrder(newpanel, newpos, whatroute, "Reverse direction") {}
    void leftclick(Vec mousepos);
};

class AddCouple : public AddOrder
{
public:
    AddCouple(Panel* newpanel, Vec newpos, Route* whatroute) : AddOrder(newpanel, newpos, whatroute, "Couple") {};
    void leftclick(Vec mousepos);
};

class AddDecouple : public AddOrder
{
public:
    AddDecouple(Panel* newpanel, Vec newpos, Route* whatroute) : AddOrder(newpanel, newpos, whatroute, "Decouple") {};
    void leftclick(Vec mousepos);
};

class AddLoadResource : public AddOrder
{
public:
    AddLoadResource(Panel* newpanel, Vec newpos, Route* whatroute) : AddOrder(newpanel, newpos, whatroute, "Load resource") {};
    void leftclick(Vec mousepos);
};

class RemoveOrder : public AddOrder
{
public:
    RemoveOrder(Panel* newpanel, Vec newpos, Route* whatroute) : AddOrder(newpanel, newpos, whatroute, "Remove selected") {};
    void leftclick(Vec mousepos);
};

} //namespace Routing
} //namespace UI