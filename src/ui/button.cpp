#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/routing/routing.h"

namespace UI{

Button::Button(Panel* newpanel, Vec newpos) : Element(newpanel)
{
    int scale = ui->getlogicalscale();
    rect = {int(newpos.x), int(newpos.y), scale*100, scale*40};
}

void Button::render(Rendering* r)
{
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderfilledrectangle(getglobalrect(), false, false);
}

TextButton::TextButton(Panel* newpanel, Vec newpos, std::string newtext, int width) : Button(newpanel, newpos)
{
    text = newtext;
    int scale = ui->getlogicalscale();
    rect.w = scale*width;
    rect.h = scale*20;
    maxtextwidth = rect.w - 10;
    SDL_Texture* tex =  loadtext(text, {0,0,0,255}, maxtextwidth);
	SDL_QueryTexture(tex, NULL, NULL, &textwidth, &textheight);
    rect.h = fmax(rect.h, 10+textheight);
	SDL_DestroyTexture(tex);
}

void TextButton::render(Rendering* r)
{
    Button::render(r);
    SDL_Rect globalrect = getglobalrect();
    r->rendertext(text, int(globalrect.x+globalrect.w*0.5-textwidth*0.5), globalrect.y+5, {255,255,255,255}, false, false, maxtextwidth);
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

void ManageRoutes::leftclick(Vec mousepos)
{
    Vec viewsize = game->getrendering().getviewsize();
    int scale = ui->getlogicalscale();
    SDL_Rect routepanelrect = {int(viewsize.x)-scale*200,0,scale*200,int(viewsize.y)};
    new RouteListPanel(ui, routepanelrect);
}

void ManageTrains::leftclick(Vec mousepos)
{
    new TrainListPanel(ui);
}

void SetRoute::leftclick(Vec mousepos)
{
    Vec panelpos = panel->topcorner();
    SDL_Rect tablerect = {int(mousepos.x-panelpos.x), int(mousepos.y-panelpos.y), 500, 200};
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
}

void GoTrain::update(int ms)
{
    bool go = dynamic_cast<TrainPanel*>(panel)->gettrain().go;
    if(go)
        text = "Stop train";
    else
        text = "Start route";
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