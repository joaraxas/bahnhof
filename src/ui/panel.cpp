#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/rendering.h"

namespace UI{

Panel::Panel(InterfaceManager* newui, SDL_Rect newrect)
{
    ui = newui;
	rect = newrect;
    ui->addpanel(this);
	buttons.emplace_back(new Close(this, Vec(20,200)));
	buttons.emplace_back(new PlaceTrack(this, Vec(20,200+30)));
	buttons.emplace_back(new PlaceSignal(this, Vec(20,200+30*2)));
}

Panel::~Panel() = default;

InterfaceManager& Panel::getui()
{
    return *ui;
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
			if(button->checkclick(mousepos, type))
				break;
		return true;
	}
    return false;
}
}