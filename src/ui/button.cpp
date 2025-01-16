#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
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

bool Button::checkclick(Vec mousepos, int type)
{
    bool clicked = Element::checkclick(mousepos, type);
	if(clicked){
        if(type==SDL_BUTTON_LEFT)
            click(mousepos);
	}
    return clicked;
}

TextButton::TextButton(Panel* newpanel, Vec newpos, std::string newtext, int width) : Element(newpanel), Button(newpanel, newpos)
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

void Close::click(Vec mousepos)
{
    panel->erase();
}

void PlaceSignal::click(Vec mousepos)
{
    game->getinputmanager().placesignal();
}

void PlaceTrack::click(Vec mousepos)
{
    game->getinputmanager().placetrack();
}

void ManageRoutes::click(Vec mousepos)
{
    Vec viewsize = game->getrendering().getviewsize();
    int scale = ui->getlogicalscale();
    SDL_Rect routepanelrect = {int(viewsize.x)-scale*200,0,scale*200,int(viewsize.y)};
    new RouteListPanel(ui, routepanelrect);
}

void ManageTrains::click(Vec mousepos)
{
    new TrainListPanel(ui);
}

namespace Routing
{

void AddTurn::click(Vec mousepos)
{
    route->insertorderatselected(new Turn());
}

void AddCouple::click(Vec mousepos)
{
    route->insertorderatselected(new Couple());
}

void AddDecouple::click(Vec mousepos)
{
    route->insertorderatselected(new Decouple());
}

void AddLoadResource::click(Vec mousepos)
{
    route->insertorderatselected(new Loadresource());
}

void RemoveOrder::click(Vec mousepos)
{
    route->removeselectedorder();
}

} //end namespace Routing

} //end namespace UI