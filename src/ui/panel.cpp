#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/routing/routing.h"

namespace UI{

Host::Host(InterfaceManager* newui, SDL_Rect newrect)
{
    ui = newui;
    game = &ui->getgame();
	rect = newrect;
}

InterfaceManager& Host::getui()
{
    return *ui;
}

void Host::erase()
{
    ui->removepanel(this);
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
	SDL_Rect absrect = ui->uitoscreen(getglobalrect());
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

Panel::Panel(InterfaceManager* newui, SDL_Rect newrect) : Host(newui, newrect)
{
    ui->addpanel(this);
	xoffset = 20;
	yoffset = 20;
	createbutton<Close>();
}

Panel::Panel(InterfaceManager* newui) : Panel::Panel(newui, {100,100,100,100}) {}

template <class T, typename... Args> void Panel::createbutton(Args&&... args){
	T* button = new T(this, Vec(xoffset,yoffset), std::forward<Args>(args)...);
	addelement(button);
	yoffset += ydist + button->getlocalrect().h;
}

void Panel::render(Rendering* r)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 127);
    r->renderfilledrectangle(ui->uitoscreen(getglobalrect()), false, false);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 127);
    r->renderrectangle(ui->uitoscreen(getglobalrect()), false, false);
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

	SDL_Rect tablerect = {110, 20, getlocalrect().w-110-10, getlocalrect().h-20-10};
	addelement(new MainInfoTable(this, tablerect));
}

RouteListPanel::RouteListPanel(InterfaceManager* newui, SDL_Rect newrect) : Panel(newui, newrect)
{
	SDL_Rect tablerect = {10, (20+30), getlocalrect().w-20, getlocalrect().h-60};
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
    SDL_Rect routepanelrect = {getlocalrect().x-300,0,300,getlocalrect().h};
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
	SDL_Rect tablerect = {10, yoffset, getlocalrect().w-20, getlocalrect().h-60};
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
    Vec viewsize = game->getrendering().getviewsize();
    float scale = ui->getlogicalscale();
    rect = {int(viewsize.x*0.5/scale-200),int(viewsize.y/scale)-200,400,200};
	SDL_Rect tablerect = {10, (20+30), getlocalrect().w-20, getlocalrect().h-60};
	addelement(new TrainTable(this, tablerect));
}

TrainListPanel::~TrainListPanel()
{
	std::cout<<"del trainlistpanel"<<std::endl;
}

TrainPanel::TrainPanel(InterfaceManager* newui, SDL_Rect newrect, Train& newtrain) : 
		Panel(newui, newrect), train(newtrain)
{
	TrainInfo info = train.getinfo();

	createbutton<SetRoute>();
	createbutton<GoTrain>();
	createbutton<GasTrain>();
	createbutton<BrakeTrain>();
	createbutton<TurnTrain>();
	
	SDL_Rect trainnamerect = {10, 10, getlocalrect().w-2*10, 20};
	trainnametext = new Text(this, info.name, trainnamerect);
	addelement(trainnametext);

	SDL_Rect traininfotablerect = {120, 40, 100, 100};
	addelement(new TrainInfoTable(this, traininfotablerect, train));

	SDL_Rect trainiconsrect = {120, (40+50), 55, 30};
	addelement(new TrainIcons(this, trainiconsrect, train));

	SDL_Rect routetablerect = {120+100+20, 40, 180, 160};
	addelement(new TrainOrderTable(this, routetablerect, train));
}

TrainPanel::~TrainPanel()
{
	std::cout<<"del trainpanel"<<std::endl;
}

} // namespace UI