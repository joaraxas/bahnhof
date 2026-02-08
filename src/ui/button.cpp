#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/buildings/buildingmanager.h"

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

} //end namespace UI