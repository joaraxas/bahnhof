#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/host.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/economy/payments.h"

class Game;
class Gamestate;
class Rendering;
class InterfaceManager;
class InputManager;
class Route;
class TrainManager;
class Train;
class Building;
class BuildingManager;
class BuildingBuilder;
class WagonFactory;
namespace Economy {
    class Stock;
    class Portfolio;
    class Account;
}

namespace Economy{
    template<typename T>
    class Control;
}

namespace UI{

class Dropdown;
class RouteListPanel;
class Text;
class Layout;

class Panel : public Host
{
public:
    Panel(InterfaceManager* newui);
    virtual void render(Rendering*);
protected:
    template <class T, typename... Args> T* create(Args&&... args);
    Layout* setlayout(Layout* l);
    Layout* getlayout();
    void applylayout(UIVec minsize);
private:
    Layout* layout{nullptr};
};

class MainPanel : public Panel
{
public:
    MainPanel(InterfaceManager* newui);
    bool usermovable() {return false;};
    void conformtorect(UIRect confrect) override;
};

class RouteListPanel : public Panel
{
public:
    RouteListPanel(InterfaceManager* newui);
};

class RoutePanel : public Panel
{
public:
    RoutePanel(InterfaceManager* newui, Route* editroute);
    void close();
    bool usermovable() {return false;};
    void conformtorect(UIRect confrect) override;
private:
    InputManager& input;
};

class TrainListPanel : public Panel
{
public:
    TrainListPanel(InterfaceManager* newui);
};

class TrainPanel : public Panel
{
public:
    TrainPanel(InterfaceManager* newui, TrainManager& manager, Train& newtrain);
    Train& gettrain() {return train;};
private:
    TrainManager& trainmanager;
    Train& train;
};

class BuildingConstructionPanel : public Panel
{
public:
    BuildingConstructionPanel(InterfaceManager* newui, const BuildingManager& bm,
        const std::vector<BuildingID>& availabletypes);
};

class BuildingPanel : public Panel
{
public:
    BuildingPanel(InterfaceManager* newui, Building& b, std::string& name);
    ~BuildingPanel();
};

class FactoryPanel : public BuildingPanel
{
public:
    FactoryPanel(InterfaceManager* newui, WagonFactory& f, std::string& name);
    ~FactoryPanel();
};

namespace EconomyPanels
{

class CompanyPanel : public Panel
{
public:
    CompanyPanel(InterfaceManager* newui, 
        Economy::Stock& stock, 
        std::string& companyname,
        std::string& slogan,
        Economy::Portfolio& portfolio,
        Economy::Account& account,
        Economy::Control<Building>& buildings);
};

class InvestorPanel : public Panel
{
    using Portfolio = Economy::Portfolio;
    using Account = Economy::Account;
public:
    InvestorPanel(InterfaceManager* newui, 
        const std::string& n, Portfolio& p, Account& a);
};

class ThePublicPanel : public Panel
{
    using Portfolio = Economy::Portfolio;
public:
    ThePublicPanel(InterfaceManager* newui, 
        const std::string& n, Portfolio& p);
};

class StockmarketPanel : public Panel
{
public:
    StockmarketPanel(InterfaceManager* newui, 
        const std::vector<Economy::Stock*>& s);
};

template<typename Possession>
class PossessionsPanel : public Panel
{
public:
    PossessionsPanel(InterfaceManager* newui, 
        const std::vector<Building*>& s);
};

class AccountPanel : public Panel
{
public:
    AccountPanel(InterfaceManager* newui, const Economy::Typelist& income, 
        const Economy::Typelist& expenses);
};

} // end namespace Economy

} // end namespace UI
