#include<iostream>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/decoration.h"

namespace UI{

Element::Element(Host* newpanel)
{
    panel = newpanel;
    ui = &panel->getui();
    game = &ui->getgame();
}

bool Element::checkclick(Vec mousepos)
{
    SDL_Rect absrect = ui->uitoscreen(getglobalrect());
	if(mousepos.x>=absrect.x && mousepos.x<=absrect.x+absrect.w && mousepos.y>=absrect.y && mousepos.y<=absrect.y+absrect.h){
		return true;
	}
    return false;
}

SDL_Rect Element::getglobalrect()
{
    SDL_Rect panelrect = panel->getglobalrect();
    return {panelrect.x+rect.x, panelrect.y+rect.y, rect.w, rect.h};
}

SDL_Rect Element::getlocalrect()
{
    return rect;
}

}