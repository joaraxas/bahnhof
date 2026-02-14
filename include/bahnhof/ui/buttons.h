#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/element.h"

class Game;
class Gamestate;
class Rendering;
class Route;
class InterfaceManager;

namespace UI{

class Button : public Element
{
public:
    Button(Host*);
    virtual ~Button() {};
    void mousehover(UIVec pos, int ms);
    virtual void render(Rendering*);
protected:
    bool clickable = true;
private:
    bool highlighted = false;
};

class TextButton : public Button
{
public:
    TextButton(Host*, std::string text, Coord width=80);
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
    Close(Host* newpanel) : 
        TextButton(newpanel, "Close") {};
    void leftclick(UIVec mousepos);
};

class PlaceSignal : public TextButton
{
public:
    PlaceSignal(Host* newpanel) : 
        TextButton(newpanel, "Build signal") {};
    void leftclick(UIVec mousepos);
};

class PlaceTrack : public TextButton
{
public:
    PlaceTrack(Host* newpanel) : 
        TextButton(newpanel, "Build track") {};
    void leftclick(UIVec mousepos);
};

class PlaceBuildings : public TextButton
{
public:
    PlaceBuildings(Host* newpanel) : 
        TextButton(newpanel, "Raise building") {};
    void leftclick(UIVec mousepos);
};

class Bulldoze : public TextButton
{
public:
    Bulldoze(Host* newpanel) : 
        TextButton(newpanel, "Bulldoze") {};
    void leftclick(UIVec mousepos);
};

class ManageRoutes : public TextButton
{
public:
    ManageRoutes(Host* newpanel) : 
        TextButton(newpanel, "Manage routes") {};
    void leftclick(UIVec mousepos);
};

class ManageTrains : public TextButton
{
public:
    ManageTrains(Host* newpanel) : 
        TextButton(newpanel, "Manage trains") {};
    void leftclick(UIVec mousepos);
};

class IncreaseUIScale : public TextButton
{
public:
    IncreaseUIScale(Host* p) : 
        TextButton(p, "Increase UI size") {};
    void leftclick(UIVec mousepos);
};

class DecreaseUIScale : public TextButton
{
public:
    DecreaseUIScale(Host* p) : 
        TextButton(p, "Decrease UI size") {};
    void leftclick(UIVec mousepos);
};

class SetRoute : public TextButton
{
public:
    SetRoute(Host* newpanel) : 
        TextButton(newpanel, "No route set") {};
    void update(int ms);
    void leftclick(UIVec mousepos);
};

class GoTrain : public TextButton
{
public:
    GoTrain(Host* newpanel) : 
        TextButton(newpanel, "Start route") {};
    void update(int ms);
    void leftclick(UIVec mousepos);
};

class GasTrain : public TextButton
{
public:
    GasTrain(Host* newpanel) : 
        TextButton(newpanel, "Gas") {};
    void render(Rendering* r) override;
    void leftpressed(UIVec mousepos, int mslogic);
};

class BrakeTrain : public TextButton
{
public:
    BrakeTrain(Host* newpanel) : 
        TextButton(newpanel, "Brake") {};
    void leftpressed(UIVec mousepos, int mslogic);
};

class TurnTrain : public TextButton
{
public:
    TurnTrain(Host* newpanel) : 
        TextButton(newpanel, "Reverse") {};
    void leftclick(UIVec mousepos);
};

class CoupleTrain : public TextButton
{
public:
    CoupleTrain(Host* newpanel) : 
        TextButton(newpanel, "Accept coupling") {};
    void update(int ms);
    void leftclick(UIVec mousepos);
};


namespace Routing
{

class AddOrder : public TextButton
{
public:
    AddOrder(Host* newpanel, Route* whatroute, std::string text) : 
        TextButton(newpanel, text, 150),
        route(whatroute) {}
protected:
    Route* route;
};

class AddTurn : public AddOrder
{
public:
    AddTurn(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, "Reverse direction") {}
    void leftclick(UIVec mousepos);
};

class AddCouple : public AddOrder
{
public:
    AddCouple(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, "Couple") {};
    void leftclick(UIVec mousepos);
};

class AddDecouple : public AddOrder
{
public:
    AddDecouple(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, "Decouple") {};
    void leftclick(UIVec mousepos);
};

class AddLoadResource : public AddOrder
{
public:
    AddLoadResource(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, "Load resource") {};
    void leftclick(UIVec mousepos);
};

class RemoveOrder : public AddOrder
{
public:
    RemoveOrder(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, "Remove selected") {};
    void leftclick(UIVec mousepos);
};

} //namespace Routing
} //namespace UI