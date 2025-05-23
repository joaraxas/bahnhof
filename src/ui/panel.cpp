#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"

namespace UI{

Panel::Panel(InterfaceManager* newui, SDL_Rect newrect) : Host(newui, newrect)
{
    ui->addpanel(this);
	createbutton<Close>();
}

Panel::Panel(InterfaceManager* newui) : Panel::Panel(newui, {100,100,100,100}) {}

template <class T, typename... Args> void Panel::createbutton(Args&&... args){
	T* button = new T(this, Vec(margin_x,margin_y+yoffset), std::forward<Args>(args)...);
	addelement(button);
	yoffset += elementdistance_y + button->getlocalrect().h;
}

void Panel::render(Rendering* r)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);
    r->renderfilledrectangle(ui->getuirendering().uitoscreen(getglobalrect()), false, false);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 127);
    r->renderrectangle(ui->getuirendering().uitoscreen(getglobalrect()), false, false);
	Host::render(r);
}

MainPanel::MainPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	createbutton<PlaceTrack>();
	createbutton<PlaceSignal>();
	createbutton<ManageRoutes>();
	createbutton<ManageTrains>();
	createbutton<IncreaseUIScale>();
	createbutton<DecreaseUIScale>();

	SDL_Rect tablerect = {margin_x+80+elementdistance_x, margin_y, getlocalrect().w-80-elementdistance_x-2*margin_x, getlocalrect().h-2*margin_y};
	addelement(new MainInfoTable(this, tablerect));
}

RouteListPanel::RouteListPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	SDL_Rect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-margin_y*2-yoffset};
	addelement(new RouteTable(this, tablerect));
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
    SDL_Rect routepanelrect = {getlocalrect().x-200,0,200,getlocalrect().h};
	routepanel = new RoutePanel(ui, routepanelrect, routeindex, this);
}

RoutePanel::RoutePanel(InterfaceManager* newui, SDL_Rect newrect, int routeid, RouteListPanel* rlp) :
	Panel(newui, newrect), routelistpanel(rlp)
{
    RouteManager& routing = game->getgamestate().getrouting();
    route = routing.getroute(routeid);
	createbutton<Routing::AddTurn>(route);
	createbutton<Routing::AddLoadResource>(route);
	createbutton<Routing::AddCouple>(route);
	createbutton<Routing::AddDecouple>(route);
	createbutton<Routing::RemoveOrder>(route);
	SDL_Rect routenamerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, 14};
	yoffset += 16;
	addelement(new EditableText(this, route->name, routenamerect));
	SDL_Rect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new OrderTable(this, tablerect, route));
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
    Vec viewsize = ui->getuirendering().screentoui(game->getrendering().getviewsize());
    rect = {int(viewsize.x*0.5-150),int(viewsize.y)-150,300,150};
	SDL_Rect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new TrainTable(this, tablerect));
}

TrainListPanel::~TrainListPanel()
{
	std::cout<<"del trainlistpanel"<<std::endl;
}

TrainPanel::TrainPanel(InterfaceManager* newui, SDL_Rect newrect, TrainManager& manager, Train& newtrain) : 
		Panel(newui, newrect), trainmanager(manager), train(newtrain)
{
	TrainInfo info = train.getinfo();

	createbutton<SetRoute>();
	createbutton<GoTrain>();
	createbutton<GasTrain>();
	createbutton<BrakeTrain>();
	createbutton<TurnTrain>();
	createbutton<CoupleTrain>();
	
	SDL_Rect trainnamerect = {margin_x, margin_y, getlocalrect().w-2*margin_x, 20};
	trainnametext = new Text(this, info.name, trainnamerect);
	addelement(trainnametext);

	int column_2_x = margin_x+80+elementdistance_x;
	int columns_y = margin_y+20+elementdistance_y;
	SDL_Rect traininfotablerect = {column_2_x, columns_y, 100, 100};
	addelement(new TrainInfoTable(this, traininfotablerect, train));

	SDL_Rect trainiconsrect = {column_2_x, columns_y+140+elementdistance_y, 200, 30};
	addelement(new TrainIcons(this, trainiconsrect, train));

	int column_3_x = column_2_x + 100 + elementdistance_x;
	SDL_Rect routetablerect = {column_3_x, columns_y, rect.w-column_3_x-margin_x, rect.h-columns_y-margin_y-35};
	addelement(new TrainOrderTable(this, routetablerect, train));
}

TrainPanel::~TrainPanel()
{
	std::cout<<"del trainpanel"<<std::endl;
}

void TrainPanel::update(int ms)
{
	if(!trainmanager.trainexists(train)){
		Panel::erase();
		return;
	}

	Panel::update(ms);
}

void TrainPanel::render(Rendering* r)
{
	if(!trainmanager.trainexists(train)){
		return;
	}

	Panel::render(r);
}

} // namespace UI