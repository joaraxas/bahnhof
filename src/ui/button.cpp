#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"

namespace UI{

Button::Button(Panel* newpanel, Vec newpos)
{
    panel = newpanel;
    ui = &panel->getui();
    game = &ui->getgame();
    int scale = ui->getlogicalscale();
    rect = {int(newpos.x), int(newpos.y), scale*100, scale*40};
}

SDL_Rect Button::getglobalrect()
{
    Vec panelpos = panel->topcorner();
    return {rect.x+int(panelpos.x), rect.y+int(panelpos.y), rect.w, rect.h};
}

SDL_Rect Button::getlocalrect()
{
    return rect;
}

void Button::render(Rendering* r)
{
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderfilledrectangle(getglobalrect(), false, false);
}

bool Button::checkclick(Vec mousepos, int type)
{
    SDL_Rect absrect = getglobalrect();
	if(mousepos.x>=absrect.x && mousepos.x<=absrect.x+absrect.w && mousepos.y>=absrect.y && mousepos.y<=absrect.y+absrect.h){
        if(type==SDL_BUTTON_LEFT)
            click();
		return true;
	}
    return false;
}

TextButton::TextButton(Panel* newpanel, Vec newpos, std::string newtext) : Button(newpanel, newpos)
{
    text = newtext;
    int scale = ui->getlogicalscale();
    rect.w = scale*80;
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

void Close::click()
{
    panel->erase();
}

void PlaceSignal::click()
{
    game->getinputmanager().placesignal();
}

void PlaceTrack::click()
{
    game->getinputmanager().placetrack();
}

void ManageRoutes::click()
{
    Vec viewsize = game->getrendering().getviewsize();
    int scale = ui->getlogicalscale();
    SDL_Rect routepanelrect = {int(viewsize.x)-scale*300,0,scale*300,int(viewsize.y)};
    new RoutePanel(ui, routepanelrect);
}

void ManageTrains::click()
{
    new TrainListPanel(ui);
}

} //end namespace UI