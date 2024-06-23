#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"

Button::Button(Panel* newpanel, Vec newpos)
{
    panel = newpanel;
    rect = {int(newpos.x), int(newpos.y), 100, 40};
}

SDL_Rect Button::getabsrect()
{
    Vec panelpos = panel->topcorner();
    return {rect.x+int(panelpos.x), rect.y+int(panelpos.y), rect.w, rect.h};
}

void Button::render(Rendering* r)
{
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderfilledrectangle(getabsrect(), false, false);
}

bool Button::click(Vec mousepos, int type)
{
    SDL_Rect absrect = getabsrect();
	if(mousepos.x>=absrect.x && mousepos.x<=absrect.x+absrect.w && mousepos.y>=absrect.y && mousepos.y<=absrect.y+absrect.h){
		std::cout<<"clicked button"<<std::endl;
        panel->erase();
		return true;
	}
    return false;
}