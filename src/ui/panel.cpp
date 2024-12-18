#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"

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

void Panel::render(Rendering* r)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);
    r->renderfilledrectangle(rect, false, false);
	for(auto& button: buttons)
		button->render(r);
}

bool Panel::click(Vec mousepos, int type)
{
	if(mousepos.x>=rect.x && mousepos.x<=rect.x+rect.w && mousepos.y>=rect.y && mousepos.y<=rect.y+rect.h){
		for(auto& button: buttons)
			if(button->checkclick(mousepos, type))
				break;
		return true;
	}
    return false;
}

Panel::~Panel(){std::cout<<"del panel"<<std::endl;}

Panel::Panel(InterfaceManager* newui, SDL_Rect newrect)
{
    ui = newui;
    game = &ui->getgame();
	rect = newrect;
    ui->addpanel(this);
}

MainPanel::MainPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	int scale = ui->getlogicalscale();
	int xoffset = 20*scale;
	int yoffset = 20*scale;
	int ydist = 10;
	buttons.emplace_back(new Close(this, Vec(xoffset,yoffset)));
	yoffset += ydist + buttons.back()->getlocalrect().h;
	buttons.emplace_back(new PlaceTrack(this, Vec(xoffset,yoffset)));
	yoffset += ydist + buttons.back()->getlocalrect().h;
	buttons.emplace_back(new PlaceSignal(this, Vec(xoffset,yoffset)));
	yoffset += ydist + buttons.back()->getlocalrect().h;
	buttons.emplace_back(new ManageRoutes(this, Vec(xoffset,yoffset)));
	yoffset += ydist + buttons.back()->getlocalrect().h;
	buttons.emplace_back(new ManageTrains(this, Vec(xoffset,yoffset)));
}

MainPanel::~MainPanel(){std::cout<<"del mainpanel"<<std::endl;}

RoutePanel::RoutePanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	int scale = ui->getlogicalscale();
	int xoffset = 20*scale;
	int yoffset = 20*scale;
	buttons.emplace_back(new Close(this, Vec(xoffset,yoffset)));
}

RoutePanel::~RoutePanel()
{
	std::cout<<"del routepanel"<<std::endl;
    game->getgamestate().getrouting().selectedroute = nullptr;
}

void RoutePanel::render(Rendering* r)
{
	Panel::render(r);
    Gamestate& gamestate = game->getgamestate();
	int scale = ui->getlogicalscale();
	int xoffset = 10*scale;
	int yoffset = (20+30)*scale;
	if(gamestate.getrouting().selectedroute)
		gamestate.getrouting().selectedroute->render(r, rect.x+xoffset, rect.y+yoffset);
	else
		gamestate.getrouting().renderroutes(r, rect.x+xoffset, rect.y+yoffset);
}

TrainPanel::TrainPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	int scale = ui->getlogicalscale();
	int xoffset = 20*scale;
	int yoffset = 20*scale;
	buttons.emplace_back(new Close(this, Vec(xoffset,yoffset)));
}

TrainPanel::~TrainPanel()
{
	game->getinputmanager().selecttrain(nullptr);
	std::cout<<"del trainpanel"<<std::endl;
}

void TrainPanel::render(Rendering* r)
{
	Panel::render(r);
	Train* selectedtrain = game->getinputmanager().getselectedtrain();
	int scale = ui->getlogicalscale();
	int xoffset = 10*scale;
	int yoffset = (20+30)*scale;
	if(selectedtrain){
		if(selectedtrain->route)
			r->rendertext(selectedtrain->route->name, rect.x + xoffset, rect.y + yoffset, {0,0,0,255}, false, false);
		else
			r->rendertext("No route assigned", rect.x + xoffset, rect.y + yoffset, {0,0,0,255}, false, false);
	}
	else
		r->rendertext("No train selected", rect.x + xoffset, rect.y + yoffset, {0,0,0,255}, false, false);
}
}