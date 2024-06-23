#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"

Panel::Panel(InterfaceManager* newui, SDL_Rect newrect)
{
    ui = newui;
	rect = newrect;
    ui->addpanel(this);
	buttons.emplace_back(new Button(this, Vec(20,20)));
}

void Panel::erase()
{
    ui->removepanel(this);
}

Vec Panel::topcorner()
{
	return Vec(rect.x, rect.y);
}

void Panel::render(Rendering* r)
{
    r->renderfilledrectangle(rect, false, false);
	for(auto& button: buttons)
		button->render(r);
}

bool Panel::click(Vec mousepos, int type)
{
	if(mousepos.x>=rect.x && mousepos.x<=rect.x+rect.w && mousepos.y>=rect.y && mousepos.y<=rect.y+rect.h){
		std::cout<<"clicked"<<std::endl;
		for(auto& button: buttons)
			if(button->click(mousepos, type))
				break;
		return true;
	}
    return false;
}