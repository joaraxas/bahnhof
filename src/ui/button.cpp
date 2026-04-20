#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/input/inputmodes.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/economy/company.h"
#include "bahnhof/economy/economymanager.h"

namespace UI{

Button::Button(Host* newpanel) : Element(newpanel)
{}

void Button::mousehover(UIVec pos, int ms)
{
    highlighted = true;
}

void Button::render(Rendering* r)
{
    TextStyle style = UI::Info;
    if(highlighted)
        style = UI::InvertedHighlighted;
    if(!clickable)
        style = UI::Inaccessible;
    ui->getuirendering().renderrectangle(r, getglobalrect(), style, true);
    highlighted = false;
}

TextButton::TextButton(Host* newpanel, 
    std::string newtext, 
    Coord width
) : Button(newpanel)
{
    text = newtext;
    rect.w = width;
    rect.h = 20;
    UIRect paddedtextrect = ui->getuirendering().gettextsize(
        text, rect, mintextoffset_x, mintextoffset_y);
    rect.h = std::max(rect.h, paddedtextrect.h);
}

void TextButton::render(Rendering* r)
{
    Button::render(r);
    ui->getuirendering().rendertext(
        r, text, getglobalrect(), InvertedInfo, true, 
        mintextoffset_x, mintextoffset_y);
}

void Close::leftclick(UIVec mousepos)
{
    panel->close();
}

void PlaceSignal::leftclick(UIVec mousepos)
{
    auto& input = game->getinputmanager();
    input.setinputmode(std::make_unique<SignalBuilder>(input, game));
}

void PlaceTrack::leftclick(UIVec mousepos)
{
    auto& input = game->getinputmanager();
    input.setinputmode(std::make_unique<TrackBuilder>(input, game));
}

void PlaceBuildings::leftclick(UIVec mousepos)
{
    game->getgamestate().getbuildingmanager().createconstructionpanel();
}

void Bulldoze::leftclick(UIVec mousepos)
{
    auto& input = game->getinputmanager();
    input.setinputmode(std::make_unique<DeleteMode>(*game));
}

void ManageRoutes::leftclick(UIVec mousepos)
{
    game->getgamestate().getrouting().createlistpanel();
}

void ManageTrains::leftclick(UIVec mousepos)
{
    game->getgamestate().gettrainmanager().createlistpanel();
}

void ManageEntity::leftclick(UIVec mousepos)
{
    game->getgamestate().controlmode.entity->createpanel(ui);
}

void ManageEntity::update(int ms)
{
    auto entity = game->getgamestate().controlmode.entity;
    text = entity->getname();
}

void SwitchControl::leftclick(UIVec mousepos)
{
    UIRect panelrect = panel->getglobalrect();
    UIVec dropdownpos = {mousepos.x-panelrect.x, 
                        mousepos.y-panelrect.y};
    new ControlDropdown(panel, dropdownpos);
}

void IncreaseUIScale::leftclick(UIVec mousepos)
{
    ui->getuirendering().increaseuiscale();
}

void DecreaseUIScale::leftclick(UIVec mousepos)
{
    ui->getuirendering().decreaseuiscale();
}

void SetRoute::leftclick(UIVec mousepos)
{
    UIRect panelrect = panel->getglobalrect();
    UIVec dropdownpos = {mousepos.x-panelrect.x, 
                        mousepos.y-panelrect.y};
    Dropdown* ntable = new RouteDropdown(panel, dropdownpos);
}

void SetRoute::update(int ms)
{
    Route* route = dynamic_cast<TrainPanel*>(panel)->gettrain().route;
    if(route)
        text = route->name;
    else
        text = "No route set";
}

void GoTrain::leftclick(UIVec mousepos)
{
    bool& go = dynamic_cast<TrainPanel*>(panel)->gettrain().go;
    go = !go;
    if(!go)
        dynamic_cast<TrainPanel*>(panel)->gettrain().speed = 0;
}

void GoTrain::update(int ms)
{
    bool go = dynamic_cast<TrainPanel*>(panel)->gettrain().go;
    if(go)
        text = "Stop train";
    else
        text = "Start route";
}

void GasTrain::render(Rendering* r)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    if(train.cangas())
        clickable = true;
    else
        clickable = false;
    TextButton::render(r);
}

void GasTrain::leftpressed(UIVec mousepos, int mslogic)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    train.gas(mslogic);
}

void BrakeTrain::leftpressed(UIVec mousepos, int mslogic)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    train.brake(mslogic);
}

void TurnTrain::leftclick(UIVec mousepos)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    train.shiftdirection();
}

void CoupleTrain::leftclick(UIVec mousepos)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    train.wantstocouple = 1 - train.wantstocouple;
}

void CoupleTrain::update(int ms)
{
    bool wantstocouple = dynamic_cast<TrainPanel*>(panel)->gettrain().wantstocouple;
    if(wantstocouple)
        text = "Reject coupling";
    else
        text = "Accept coupling";
}

namespace Routing
{

void AddTurn::leftclick(UIVec mousepos)
{
    route->insertorderatselected(new Turn());
}

void AddCouple::leftclick(UIVec mousepos)
{
    route->insertorderatselected(new Couple());
}

void AddDecouple::leftclick(UIVec mousepos)
{
    route->insertorderatselected(new Decouple());
}

void AddLoadResource::leftclick(UIVec mousepos)
{
    route->insertorderatselected(new Loadresource());
}

void RemoveOrder::leftclick(UIVec mousepos)
{
    route->removeselectedorder();
}

} //end namespace Routing

namespace EconomyPanels
{

void PublicOffering::update(int ms)
{
    if(playercontrol.is){
        clickable = true;
    }
    else{
        clickable = false;
    }
}

void PublicOffering::leftclick(UIVec mousepos)
{
    if(playercontrol.is)
        stock.issue(100, game->getgamestate().geteconomymanager().thepublic.getinvestments());
}


void TakeOver::leftclick(UIVec mousepos)
{
    if(playercontrol.is) return;
    bool succeeded = stock.attempttakeover();
    if(succeeded) {
        playercontrol.is = true;
        game->getgamestate().addcontrolmode(controlmode);
    }
}


void Buy::leftclick(UIVec mousepos)
{
    game->getgamestate().controlmode.portfolio->buy(
        game->getgamestate().geteconomymanager().thepublic.getinvestments(),
        stock, 5
    );
}

void Sell::leftclick(UIVec mousepos)
{
    game->getgamestate().geteconomymanager().thepublic.getinvestments().buy(
        *game->getgamestate().controlmode.portfolio,
        stock, 5
    );
}

void VisitStockmarket::leftclick(UIVec mousepos)
{
    game->getgamestate().geteconomymanager().stockmarket.createpanel(ui);
}

void ListBuildings::leftclick(UIVec mousepos)
{
    list.createpanel(ui);
}

void ShowAccounts::leftclick(UIVec mousepos)
{
    account.createpanel(ui);
}

} // end namespace EconomyPanels

Trade::Trade(Host* newpanel, Building& b) :
        TextButton{newpanel, "Buy\n(" + std::string(b.getvalue()) + ")"}, building{b}
{
    BuildingOwner* playerownership = game->getgamestate().controlmode.buildings;
    updatetext(&building.getowner() == playerownership);
    if(playerownership==nullptr){ // can't own buildings
        clickable = false;
    }
}

void Trade::mousehover(UIVec pos, int ms)
{
    if(!game->getgamestate().controlmode.buildings){
        ui->addtooltip("Can't own buildings, switch user mode to e.g. a company.");
        return;
    }
    TextButton::mousehover(pos, ms);
}

void Trade::leftclick(UIVec mousepos)
{
    BuildingOwner* playerownership = game->getgamestate().controlmode.buildings;
    if(!playerownership){
        clickable = false;
        return;
    }
    bool wasplayerowned = true;
    BuildingOwner* buyer = &game->getgamestate().geteconomymanager().
        thepublic.getbuildings();
    if(&building.getowner() != playerownership){
        buyer = playerownership;
        wasplayerowned = false;
    }
    if(!buy(building, *buyer, building.getvalue())) return;
    updatetext(!wasplayerowned);
}

void Trade::updatetext(bool isplayerowned)
{
    if(isplayerowned)
        text = "Sell\n(" + std::string(building.getvalue()) + ")";
    else
        text = "Buy\n(" + std::string(building.getvalue()) + ")";
}

} //end namespace UI