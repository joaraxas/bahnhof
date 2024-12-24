#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/common/timing.h"

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


template <class T> void Panel::addbutton(){
	T* button = new T(this, Vec(xoffset,yoffset));
	buttons.emplace_back(button);
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
	addbutton<Close>();
}

MainPanel::MainPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	addbutton<PlaceTrack>();
	addbutton<PlaceSignal>();
	addbutton<ManageRoutes>();
	addbutton<ManageTrains>();
}

MainPanel::~MainPanel(){std::cout<<"del mainpanel"<<std::endl;}

void MainPanel::render(Rendering* r)
{
	Panel::render(r);
    Gamestate& gamestate = game->getgamestate();
    int scale = r->getlogicalscale();
	r->rendertable({std::to_string(int(gamestate.money)) + " Fr",
                    std::to_string(int(gamestate.time*0.001/60)) + " min", 
                    std::to_string(int(60*float(gamestate.revenue)/float(gamestate.time*0.001/60))) + " Fr/h",
                    std::to_string(game->gettimemanager().getfps()) + " fps"}, 
                   {110*scale, 28*scale, 100*scale, 2000*scale});
}

RoutePanel::RoutePanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
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
} // namespace UI