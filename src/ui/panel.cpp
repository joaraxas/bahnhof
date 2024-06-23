#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"

Panel::Panel(InterfaceManager* newui, SDL_Rect newrect)
{
    ui = newui;
	rect = newrect;
}

void Panel::render(Rendering* r)
{
    r->renderfilledrectangle(rect, false, false);
}

bool Panel::click(Vec mousepos, int type)
{
	if(mousepos.x>=rect.x && mousepos.x<=rect.x+rect.w && mousepos.y>=rect.y && mousepos.y<=rect.y+rect.h){
		std::cout<<"clicked"<<std::endl;
		return true;
	}
    return false;
}