#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/element.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/input/controlmode.h"
#include <localization/localization.h>

class Game;
class Gamestate;
class Rendering;
class Route;
class InterfaceManager;

namespace Economy{
    class ControlMode;
}

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
        TextButton(newpanel, tr("button.close")) {};
    void leftclick(UIVec mousepos);
};

class PlaceSignal : public TextButton
{
public:
    PlaceSignal(Host* newpanel) : 
        TextButton(newpanel, tr("button.buildsignal")) {};
    void leftclick(UIVec mousepos);
};

class PlaceTrack : public TextButton
{
public:
    PlaceTrack(Host* newpanel) : 
        TextButton(newpanel, tr("button.buildtrack")) {};
    void leftclick(UIVec mousepos);
};

class PlaceBuildings : public TextButton
{
public:
    PlaceBuildings(Host* newpanel) : 
        TextButton(newpanel, tr("button.buildbuilding")) {};
    void leftclick(UIVec mousepos);
};

class Bulldoze : public TextButton
{
public:
    Bulldoze(Host* newpanel) : 
        TextButton(newpanel, tr("button.bulldoze")) {};
    void leftclick(UIVec mousepos);
};

class ManageRoutes : public TextButton
{
public:
    ManageRoutes(Host* newpanel) : 
        TextButton(newpanel, tr("button.routes")) {};
    void leftclick(UIVec mousepos);
};

class ManageTrains : public TextButton
{
public:
    ManageTrains(Host* newpanel) : 
        TextButton(newpanel, tr("button.trains")) {};
    void leftclick(UIVec mousepos);
};

class ManageEntity : public TextButton
{
public:
    ManageEntity(Host* newpanel) : 
        TextButton(newpanel, tr("button.currententity")) {};
    void leftclick(UIVec mousepos);
    void update(int ms);
};

class SwitchControl : public TextButton
{
public:
    SwitchControl(Host* newpanel) : 
        TextButton(newpanel, tr("button.switchuser"))
        {};
    void leftclick(UIVec mousepos);
};

class IncreaseUIScale : public TextButton
{
public:
    IncreaseUIScale(Host* p) : 
        TextButton(p, tr("button.increaseuiscale")) {};
    void leftclick(UIVec mousepos);
};

class DecreaseUIScale : public TextButton
{
public:
    DecreaseUIScale(Host* p) : 
        TextButton(p, tr("button.decreaseuiscale")) {};
    void leftclick(UIVec mousepos);
};

class SetRoute : public TextButton
{
public:
    SetRoute(Host* newpanel) : 
        TextButton(newpanel, tr("button.train.noroute")) {};
    void update(int ms);
    void leftclick(UIVec mousepos);
};

class GoTrain : public TextButton
{
public:
    GoTrain(Host* newpanel) : 
        TextButton(newpanel, tr("button.train.go")) {};
    void update(int ms);
    void leftclick(UIVec mousepos);
};

class GasTrain : public TextButton
{
public:
    GasTrain(Host* newpanel) : 
        TextButton(newpanel, tr("button.train.gas")) {};
    void render(Rendering* r) override;
    void leftpressed(UIVec mousepos, int mslogic);
};

class BrakeTrain : public TextButton
{
public:
    BrakeTrain(Host* newpanel) : 
        TextButton(newpanel, tr("button.train.brake")) {};
    void leftpressed(UIVec mousepos, int mslogic);
};

class TurnTrain : public TextButton
{
public:
    TurnTrain(Host* newpanel) : 
        TextButton(newpanel, tr("button.train.reverse")) {};
    void leftclick(UIVec mousepos);
};

class CoupleTrain : public TextButton
{
public:
    CoupleTrain(Host* newpanel) : 
        TextButton(newpanel, tr("button.train.acceptcoupling")) {};
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
        AddOrder(newpanel, whatroute, tr("button.order.reverse")) {}
    void leftclick(UIVec mousepos);
};

class AddCouple : public AddOrder
{
public:
    AddCouple(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, tr("button.order.couple")) {};
    void leftclick(UIVec mousepos);
};

class AddDecouple : public AddOrder
{
public:
    AddDecouple(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, tr("button.order.decouple")) {};
    void leftclick(UIVec mousepos);
};

class AddLoadResource : public AddOrder
{
public:
    AddLoadResource(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, tr("button.order.loadunload")) {};
    void leftclick(UIVec mousepos);
};

class RemoveOrder : public AddOrder
{
public:
    RemoveOrder(Host* newpanel, Route* whatroute) : 
        AddOrder(newpanel, whatroute, tr("button.order.remove")) {};
    void leftclick(UIVec mousepos);
};

} // end namespace Routing

namespace EconomyPanels
{
using Stock = Economy::Stock;
using PlayerControl = Economy::PlayerControl;
using ControlMode = Economy::ControlMode;

class PublicOffering : public TextButton
{
public:
    PublicOffering(Host* newpanel, Stock& s, PlayerControl& c) : 
        TextButton(newpanel, tr("button.stocks.equityoffering"), 120), 
        stock{s}, playercontrol{c} {};
    virtual void update(int ms) override;
    void leftclick(UIVec mousepos);
private:
    Stock& stock;
    PlayerControl& playercontrol;
};

class TakeOver : public TextButton
{
public:
    TakeOver(Host* newpanel, Stock& s, PlayerControl& c, Entity** chairptr,
            ControlMode mode) : 
        TextButton(newpanel, tr("button.stocks.takeover"), 120), stock{s}, 
        playercontrol{c}, chairmanptr{chairptr}, controlmode{mode} {};
    virtual void update(int ms) override;
    void leftclick(UIVec mousepos);
    void mousehover(UIVec pos, int ms);
private:
    Entity* const getchallenger() const;
    bool cantakeover(const Entity* const challenger) const;
    Stock& stock;
    PlayerControl& playercontrol;
    Entity** chairmanptr;
    ControlMode controlmode;
};

class Buy : public TextButton
{
    static constexpr int numshares = 5;
public:
    Buy(Host* newpanel, Stock& s) : 
        TextButton(newpanel, tr("button.stocks.buy", numshares), 120), stock{s} {};
    void leftclick(UIVec mousepos);
private:
    Stock& stock;
};

class Sell : public TextButton
{
    static constexpr int numshares = 5;
public:
    Sell(Host* newpanel, Stock& s) : 
        TextButton(newpanel, tr("button.stocks.sell", numshares), 120), stock{s} {};
    void leftclick(UIVec mousepos);
private:
    Stock& stock;
};

class VisitStockmarket : public TextButton
{
public:
    VisitStockmarket(Host* newpanel) : 
        TextButton(newpanel, tr("button.visitstockmarket")) {};
    void leftclick(UIVec mousepos);
};

class ListBuildings : public TextButton
{
public:
    ListBuildings(Host* newpanel, Economy::Control<Building>& l) : 
        TextButton(newpanel, tr("button.listbuildings")), list{l} {};
    void leftclick(UIVec mousepos);
private:
    Economy::Control<Building>& list;
};

class ShowAccounts : public TextButton
{
public:
    ShowAccounts(Host* newpanel, Economy::Account& a) : 
        TextButton(newpanel, tr("button.accounting")), account{a} {};
    void leftclick(UIVec mousepos);
private:
    Economy::Account& account;
};

} // end namespace Economy

class Trade : public TextButton
{
public:
    Trade(Host* newpanel, Building& b);
    void leftclick(UIVec mousepos);
    void mousehover(UIVec pos, int ms);
private:
    void updatetext(bool isplayerowned);
    Building& building;
};

} // end namespace UI