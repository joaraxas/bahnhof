#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/panels.h"

namespace UI{

Host::Host(InterfaceManager* newui, SDL_Rect newrect)
{
    ui = newui;
    game = &ui->getgame();
	rect = newrect;
}

Host::~Host()
{}

InterfaceManager& Host::getui()
{
    return *ui;
}

void Host::erase()
{
    ui->removepanel(this);
	if(owner){
		owner->resetreference();
	}
}

SDL_Rect Host::getglobalrect()
{
	return rect;
}

SDL_Rect Host::getlocalrect()
{
	return rect;
}

void Host::update(int ms)
{
	for(auto& element: elements)
		element->update(ms);
}

void Host::render(Rendering* r)
{
	for(auto& element: elements)
		element->render(r);
}

bool Host::checkclick(Vec pos)
{
	SDL_Rect absrect = ui->getuirendering().uitoscreen(getglobalrect());
	if(pos.x>=absrect.x && pos.x<=absrect.x+absrect.w && pos.y>=absrect.y && pos.y<=absrect.y+absrect.h){
		return true;
	}
    return false;
}

Element* Host::getelementat(Vec pos)
{
	for(auto& element: elements)
		if(element->checkclick(pos)){
			return element.get();
		}
	return nullptr;
}

void Host::mousehover(Vec pos, int ms)
{
	Element* hoveredelement = getelementat(pos);
	if(hoveredelement){
		hoveredelement->mousehover(pos, ms);
	}
}

void Host::click(Vec pos, int type)
{
	Element* clickedelement = getelementat(pos);
	if(clickedelement){
		switch (type)
		{
		case SDL_BUTTON_LEFT:
			clickedelement->leftclick(pos);
			break;
		}
	}
}

void Host::scroll(Vec pos, int distance)
{
	Element* clickedelement = getelementat(pos);
	if(clickedelement){
		clickedelement->scroll(pos, distance);
	}
}

void Host::mousepress(Vec pos, int mslogic, int type)
{
	Element* clickedelement = getelementat(pos);
	if(clickedelement){
		switch (type)
		{
		case SDL_BUTTON_LEFT:
			clickedelement->leftpressed(pos, mslogic);
			break;
		}
	}
}

void Host::addelement(Element* element){
	elements.emplace_back(element);
}

void Host::move(Vec towhattopcorner){
	rect.x = round(towhattopcorner.x);
	rect.y = round(towhattopcorner.y);
}

} // namespace UI