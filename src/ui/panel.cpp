#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/routing/routing.h"

namespace UI{

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

void Panel::update(int ms)
{
	for(auto& element: elements)
		element->update(ms);
}

void Panel::render(Rendering* r)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);
    r->renderfilledrectangle(rect, false, false);
	for(auto& element: elements)
		element->render(r);
}

bool Panel::click(Vec mousepos, int type)
{
	if(mousepos.x>=rect.x && mousepos.x<=rect.x+rect.w && mousepos.y>=rect.y && mousepos.y<=rect.y+rect.h){
		for(auto& element: elements)
			if(element->checkclick(mousepos, type))
				break;
		return true;
	}
    return false;
}

void Panel::addelement(Element* element){
	elements.emplace_back(element);
}

template <class T> void Panel::createbutton(){
	T* button = new T(this, Vec(xoffset,yoffset));
	addelement(button);
	yoffset += ydist + button->getlocalrect().h;
}

Panel::~Panel(){std::cout<<"del panel"<<std::endl;}

Panel::Panel(InterfaceManager* newui, SDL_Rect newrect)
{
    ui = newui;
    game = &ui->getgame();
	rect = newrect;
    ui->addpanel(this);
	
	int scale = ui->getlogicalscale();
	xoffset = 20*scale;
	yoffset = 20*scale;
	createbutton<Close>();
}

Panel::Panel(InterfaceManager* newui) : Panel::Panel(newui, {100,100,100,100}){}

MainPanel::MainPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	createbutton<PlaceTrack>();
	createbutton<PlaceSignal>();
	createbutton<ManageRoutes>();
	createbutton<ManageTrains>();

    int scale = ui->getlogicalscale();
	SDL_Rect tablerect = {110*scale, 28*scale, 100*scale, 2000*scale};
	addelement(new MainInfoTable(this, tablerect));
}

RouteListPanel::RouteListPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
    int scale = ui->getlogicalscale();
	SDL_Rect tablerect = {10*scale, (20+30)*scale, rect.w-20*scale, rect.h-60*scale};
	addelement(new RouteListTable(this, tablerect));
}

RouteListPanel::~RouteListPanel()
{
	std::cout<<"del routelistpanel"<<std::endl;
}

void RouteListPanel::erase()
{
	if(routepanel)
		routepanel->erase();
	Panel::erase();
}

void RouteListPanel::addroutepanel(int routeindex)
{
	if(routepanel)
		routepanel->erase();
    Vec viewsize = game->getrendering().getviewsize();
    int scale = ui->getlogicalscale();
    SDL_Rect routepanelrect = {int(viewsize.x)-scale*500,0,scale*300,int(viewsize.y)};
	routepanel = new RoutePanel(ui, routepanelrect, routeindex, this);
}

RoutePanel::RoutePanel(InterfaceManager* newui, SDL_Rect newrect, int routeid, RouteListPanel* rlp) :
	Panel(newui, newrect), routelistpanel(rlp)
{
    RouteManager& routing = game->getgamestate().getrouting();
    route = routing.getroute(routeid);
    int scale = ui->getlogicalscale();
	SDL_Rect tablerect = {10*scale, (20+30)*scale, rect.w-20*scale, rect.h-60*scale};
	addelement(new RouteTable(this, tablerect, route));
	game->getinputmanager().editroute(route);
}

RoutePanel::~RoutePanel()
{
	std::cout<<"del routepanel"<<std::endl;
}

void RoutePanel::erase()
{
	if(routelistpanel)
		routelistpanel->deselectroutepanel();
    game->getinputmanager().editroute(nullptr);
	Panel::erase();
}

TrainListPanel::TrainListPanel(InterfaceManager* newui) : Panel(newui)
{
    Vec viewsize = game->getrendering().getviewsize();
    int scale = ui->getlogicalscale();
    rect = {scale*300,int(viewsize.y)-scale*200,scale*400,scale*200};
	SDL_Rect tablerect = {10*scale, (20+30)*scale, rect.w-20*scale, rect.h-60*scale};
	addelement(new TrainTable(this, tablerect));
}

TrainListPanel::~TrainListPanel()
{
	std::cout<<"del trainlistpanel"<<std::endl;
}

// TrainPanel::TrainPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
// {
// }

// TrainPanel::~TrainPanel()
// {
// 	game->getinputmanager().selecttrain(nullptr);
// 	std::cout<<"del trainpanel"<<std::endl;
// }

// void TrainPanel::render(Rendering* r)
// {
// 	Panel::render(r);
// 	Train* selectedtrain = game->getinputmanager().getselectedtrain();
// 	int scale = ui->getlogicalscale();
// 	int xoffset = 10*scale;
// 	int yoffset = (20+30)*scale;
// 	if(selectedtrain){
// 		if(selectedtrain->route)
// 			r->rendertext(selectedtrain->route->name, rect.x + xoffset, rect.y + yoffset, {0,0,0,255}, false, false);
// 		else
// 			r->rendertext("No route assigned", rect.x + xoffset, rect.y + yoffset, {0,0,0,255}, false, false);
// 	}
// 	else
// 		r->rendertext("No train selected", rect.x + xoffset, rect.y + yoffset, {0,0,0,255}, false, false);
// }
} // namespace UI