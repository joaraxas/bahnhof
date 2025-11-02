#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"

namespace UI{

Button::Button(Host* newpanel, Vec newpos) : Element(newpanel)
{
    rect = {int(newpos.x), int(newpos.y), 100, 40};
}

void Button::mousehover(Vec pos, int ms)
{
    highlighted = true;
}

void Button::render(Rendering* r)
{
    int i = highlighted*127;
    SDL_SetRenderDrawColor(renderer,i,i,i,255);
    r->renderfilledrectangle(ui->getuirendering().uitoscreen(getglobalrect()), false, false);
    highlighted = false;
}

TextButton::TextButton(Host* newpanel, Vec newpos, std::string newtext, int width) : Button(newpanel, newpos)
{
    text = newtext;
    rect.w = width;
    rect.h = 20;
    UIRect paddedtextrect = ui->getuirendering().gettextsize(text, rect, mintextoffset_x, mintextoffset_y);
    rect.h = fmax(rect.h, double(paddedtextrect.h));
}

void TextButton::render(Rendering* r)
{
    Button::render(r);
    ui->getuirendering().rendertext(r, text, getglobalrect(), InvertedInfo, true, mintextoffset_x, mintextoffset_y);
}

void Close::leftclick(Vec mousepos)
{
    panel->erase();
}

void PlaceSignal::leftclick(Vec mousepos)
{
    game->getinputmanager().placesignal();
}

void PlaceTrack::leftclick(Vec mousepos)
{
    game->getinputmanager().placetrack();
}

void PlaceBuildings::leftclick(Vec mousepos)
{
    new BuildingConstructionPanel(ui, {180,0,300,200});
}

void ManageRoutes::leftclick(Vec mousepos)
{
    Vec viewsize = ui->getuirendering().screentoui(getviewsize());
    SDL_Rect routepanelrect = {int(viewsize.x)-200,0,200,int(viewsize.y)};
    new RouteListPanel(ui, routepanelrect);
}

void ManageTrains::leftclick(Vec mousepos)
{
    new TrainListPanel(ui);
}

void IncreaseUIScale::leftclick(Vec mousepos)
{
    ui->getuirendering().increaseuiscale();
}

void DecreaseUIScale::leftclick(Vec mousepos)
{
    ui->getuirendering().decreaseuiscale();
}

void SetRoute::leftclick(Vec mousepos)
{
    SDL_Rect panelrect = panel->getlocalrect();
    Vec uimousepos = ui->getuirendering().screentoui(mousepos);
    SDL_Rect tablerect = {int(uimousepos.x-panelrect.x), int(uimousepos.y-panelrect.y), 150, 100};
    Dropdown* ntable = new RouteDropdown(panel, tablerect);
}

void SetRoute::update(int ms)
{
    Route* route = dynamic_cast<TrainPanel*>(panel)->gettrain().route;
    if(route)
        text = route->name;
    else
        text = "No route set";
}

void GoTrain::leftclick(Vec mousepos)
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

void GasTrain::leftpressed(Vec mousepos, int mslogic)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    train.gas(mslogic);
}

void BrakeTrain::leftpressed(Vec mousepos, int mslogic)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    train.brake(mslogic);
}

void TurnTrain::leftclick(Vec mousepos)
{
    Train& train = dynamic_cast<TrainPanel*>(panel)->gettrain();
    train.shiftdirection();
}

void CoupleTrain::leftclick(Vec mousepos)
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

void AddTurn::leftclick(Vec mousepos)
{
    route->insertorderatselected(new Turn());
}

void AddCouple::leftclick(Vec mousepos)
{
    route->insertorderatselected(new Couple());
}

void AddDecouple::leftclick(Vec mousepos)
{
    route->insertorderatselected(new Decouple());
}

void AddLoadResource::leftclick(Vec mousepos)
{
    route->insertorderatselected(new Loadresource());
}

void RemoveOrder::leftclick(Vec mousepos)
{
    route->removeselectedorder();
}

} //end namespace Routing

} //end namespace UI