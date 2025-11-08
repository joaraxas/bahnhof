#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/host.h"
#include "bahnhof/ui/element.h"

namespace UI{

Ownership::~Ownership()
{
	deletereference();
}

void Ownership::set(Host* newhost)
{
	deletereference();
	host = newhost;
	host->owner = this;
}

bool Ownership::exists()
{
	return host!=nullptr;
}

void Ownership::deletereference()
{
	if(host){
		host->erase();
		resetreference();
	}
}

void Ownership::resetreference()
{
	host = nullptr;
}


Host::Host(InterfaceManager* newui, UIRect newrect) :
	rect{newrect}
{
    ui = newui;
    game = &ui->getgame();
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

UIRect Host::getglobalrect()
{
	return rect;
}

UIRect Host::getlocalrect()
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

Element* Host::getelementat(UIVec pos)
{
	for(auto& element: elements)
		if(element->checkclick(pos)){
			return element.get();
		}
	return nullptr;
}

void Host::mousehover(UIVec pos, int ms)
{
	Element* hoveredelement = getelementat(pos);
	if(hoveredelement){
		hoveredelement->mousehover(pos, ms);
	}
}

void Host::click(UIVec pos, int type)
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

void Host::scroll(UIVec pos, int distance)
{
	Element* clickedelement = getelementat(pos);
	if(clickedelement){
		clickedelement->scroll(pos, distance);
	}
}

void Host::mousepress(UIVec pos, int mslogic, int type)
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

void Host::moveto(UIVec towhattopcorner){
	rect.x = towhattopcorner.x;
	rect.y = towhattopcorner.y;
}

} // namespace UI