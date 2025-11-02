#pragma once
#include "bahnhof/common/math.h"
#include "bahnhof/ui/ui.h"

class Game;
class Gamestate;
class Rendering;
class Route;
class InterfaceManager;

namespace UI{

class Button : public Element
{
public:
    Button(Host*, UIVec newpos);
    virtual ~Button() {};
    void mousehover(Vec pos, int ms);
    virtual void render(Rendering*);
private:
    bool highlighted = false;
};

class TextButton : public Button
{
public:
    TextButton(Host*, UIVec newpos, std::string text, Coord width=80);
    virtual ~TextButton() {};
    virtual void render(Rendering*);
protected:
    std::string text;
    static constexpr Coord mintextoffset_x{6};
    static constexpr Coord mintextoffset_y{4};
};

class Close : public TextButton
{
public:
    Close(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Close") {};
    void leftclick(Vec mousepos);
};

class PlaceSignal : public TextButton
{
public:
    PlaceSignal(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Build signal") {};
    void leftclick(Vec mousepos);
};

class PlaceTrack : public TextButton
{
public:
    PlaceTrack(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Build track") {};
    void leftclick(Vec mousepos);
};

class PlaceBuildings : public TextButton
{
public:
    PlaceBuildings(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Raise building") {};
    void leftclick(Vec mousepos);
};

class ManageRoutes : public TextButton
{
public:
    ManageRoutes(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Manage routes") {};
    void leftclick(Vec mousepos);
};

class ManageTrains : public TextButton
{
public:
    ManageTrains(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Manage trains") {};
    void leftclick(Vec mousepos);
};

class IncreaseUIScale : public TextButton
{
public:
    IncreaseUIScale(Host* p, Vec pos) : 
        TextButton(p, pos, "Increase UI size") {};
    void leftclick(Vec mousepos);
};

class DecreaseUIScale : public TextButton
{
public:
    DecreaseUIScale(Host* p, Vec pos) : 
        TextButton(p, pos, "Decrease UI size") {};
    void leftclick(Vec mousepos);
};

class SetRoute : public TextButton
{
public:
    SetRoute(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "No route set") {};
    void update(int ms);
    void leftclick(Vec mousepos);
};

class GoTrain : public TextButton
{
public:
    GoTrain(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Start route") {};
    void update(int ms);
    void leftclick(Vec mousepos);
};

class GasTrain : public TextButton
{
public:
    GasTrain(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Gas") {};
    void leftpressed(Vec mousepos, int mslogic);
};

class BrakeTrain : public TextButton
{
public:
    BrakeTrain(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Brake") {};
    void leftpressed(Vec mousepos, int mslogic);
};

class TurnTrain : public TextButton
{
public:
    TurnTrain(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Reverse") {};
    void leftclick(Vec mousepos);
};

class CoupleTrain : public TextButton
{
public:
    CoupleTrain(Host* newpanel, UIVec newpos) : 
        TextButton(newpanel, newpos, "Accept coupling") {};
    void update(int ms);
    void leftclick(Vec mousepos);
};


namespace Routing
{

class AddOrder : public TextButton
{
public:
    AddOrder(Host* newpanel, UIVec newpos, Route* whatroute, std::string text) : 
        TextButton(newpanel, newpos, text, 120),
        route(whatroute) {}
protected:
    Route* route;
};

class AddTurn : public AddOrder
{
public:
    AddTurn(Host* newpanel, UIVec newpos, Route* whatroute) : 
        AddOrder(newpanel, newpos, whatroute, "Reverse direction") {}
    void leftclick(Vec mousepos);
};

class AddCouple : public AddOrder
{
public:
    AddCouple(Host* newpanel, UIVec newpos, Route* whatroute) : 
        AddOrder(newpanel, newpos, whatroute, "Couple") {};
    void leftclick(Vec mousepos);
};

class AddDecouple : public AddOrder
{
public:
    AddDecouple(Host* newpanel, UIVec newpos, Route* whatroute) : 
        AddOrder(newpanel, newpos, whatroute, "Decouple") {};
    void leftclick(Vec mousepos);
};

class AddLoadResource : public AddOrder
{
public:
    AddLoadResource(Host* newpanel, UIVec newpos, Route* whatroute) : 
        AddOrder(newpanel, newpos, whatroute, "Load resource") {};
    void leftclick(Vec mousepos);
};

class RemoveOrder : public AddOrder
{
public:
    RemoveOrder(Host* newpanel, UIVec newpos, Route* whatroute) : 
        AddOrder(newpanel, newpos, whatroute, "Remove selected") {};
    void leftclick(Vec mousepos);
};

} //namespace Routing
} //namespace UI