#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/ownership.h"
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


Host::Host(InterfaceManager* newui)
{
    ui = newui;
	ui->addpanel(this);
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
	return {0,0,rect.w,rect.h};
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

Element* Host::addelement(Element* element){
	elements.emplace_back(element);
	return element;
}

void Host::moveto(UIVec towhattopcorner){
	rect.x = towhattopcorner.x;
	rect.y = towhattopcorner.y;
}

void Host::placeautomatically(){
	UIVec size{rect.w, rect.h};
	moveto(ui->findfreespace(size));
}

void Host::conformtorect(UIRect confrect){
	UIVec newpos{rect.x,rect.y};
	if(rect.x + 20 > confrect.x+confrect.w)
		newpos.x = confrect.x+confrect.w - rect.w;
	if(rect.y + 20 > confrect.y+confrect.h)
		newpos.y = confrect.y+confrect.h - rect.h;
	if(rect.x + rect.w < confrect.x + 20)
		newpos.x = confrect.x;
	if(rect.y + rect.h < confrect.y + 20)
		newpos.y = confrect.y;
	moveto(newpos);
}

} // namespace UI