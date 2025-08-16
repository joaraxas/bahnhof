#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"

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


MainPanel::MainPanel(InterfaceManager* newui) : Panel(newui)
{
	createbutton<PlaceTrack>();
	createbutton<PlaceSignal>();
	createbutton<PlaceBuildings>();
	createbutton<ManageRoutes>();
	createbutton<ManageTrains>();
	createbutton<IncreaseUIScale>();
	createbutton<DecreaseUIScale>();
	rect = {0,0,180,yoffset + 2*margin_y};

	SDL_Rect tablerect = {margin_x+80+elementdistance_x, margin_y, getlocalrect().w-80-elementdistance_x-2*margin_x, getlocalrect().h-2*margin_y};
	addelement(new MainInfoTable(this, tablerect));
}

MainPanel::~MainPanel()
{
	std::cout<<"del mainpanel"<<std::endl;
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
    Vec viewsize = getviewsize();
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
	SDL_Rect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new OrderTable(this, tablerect, route));
	addelement(new EditableText(this, route->name, routenamerect));
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
    Vec viewsize = ui->getuirendering().screentoui(getviewsize());
    rect = {int(viewsize.x*0.5-150),int(viewsize.y)-150,300,150};
	SDL_Rect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new TrainTable(this, tablerect));
}

TrainListPanel::~TrainListPanel()
{
	std::cout<<"del trainlistpanel"<<std::endl;
}


TrainPanel::TrainPanel(InterfaceManager* newui, SDL_Rect newrect, TrainManager& manager, Train& newtrain) : 
		Panel(newui, newrect), 
		trainmanager(manager), 
		train(newtrain)
{
	createbutton<SetRoute>();
	createbutton<GoTrain>();
	createbutton<GasTrain>();
	createbutton<BrakeTrain>();
	createbutton<TurnTrain>();
	createbutton<CoupleTrain>();

	int column_2_x = margin_x+80+elementdistance_x;
	int columns_y = margin_y+20+elementdistance_y;

	SDL_Rect traininfotablerect = {column_2_x, columns_y, 100, 100};
	addelement(new TrainInfoTable(this, traininfotablerect, train));

	SDL_Rect trainiconsrect = {column_2_x, columns_y+140+elementdistance_y, 200, 30};
	addelement(new TrainIcons(this, trainiconsrect, train));

	int column_3_x = column_2_x + 100 + elementdistance_x;
	SDL_Rect routetablerect = {column_3_x, columns_y, rect.w-column_3_x-margin_x, rect.h-columns_y-margin_y-35};
	addelement(new TrainOrderTable(this, routetablerect, train));
	
	SDL_Rect trainnamerect = {column_2_x, margin_y, getlocalrect().w-2*column_2_x, 20};
	addelement(new EditableText(this, train.name, trainnamerect));
}

TrainPanel::~TrainPanel()
{
	std::cout<<"del trainpanel"<<std::endl;
}

void TrainPanel::update(int ms)
{
	if(!trainmanager.trainexists(train)){
		Panel::erase();
		return; // TODO: Replace these functions with the Owner-reference system
	}

	Panel::update(ms);
}

void TrainPanel::render(Rendering* r)
{
	if(!trainmanager.trainexists(train)){
		return; // TODO: Replace these functions with the Owner-reference system
	}

	Panel::render(r);
}


BuildingConstructionPanel::BuildingConstructionPanel(InterfaceManager* newui, SDL_Rect r) : Panel(newui, r)
{
	SDL_Rect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-margin_y-yoffset-elementdistance_y};
	addelement(new ConstructionTable(this, tablerect));
}

BuildingConstructionPanel::~BuildingConstructionPanel()
{
	std::cout<<"del buildingconstructionpanel"<<std::endl;
}

void BuildingConstructionPanel::erase()
{
	game->getinputmanager().resetinput();
	Panel::erase();
}

BuildingPanel::BuildingPanel(InterfaceManager* newui, Building* b) : 
		Panel(newui, {200,400,500,150}),
		building(b)
{
	int column_2_x = margin_x + 80 + elementdistance_x;
	int typenamewidth = 80;
	int column_3_x = rect.w - typenamewidth - margin_x;
	int namewidth = column_3_x - column_2_x - elementdistance_x;
	addelement(new EditableText(this, building->name, {column_2_x, 10, namewidth, 20}));
	addelement(new Text(this, building->type.name, {column_3_x, 10, typenamewidth, 20}));
}

BuildingPanel::~BuildingPanel()
{
	std::cout<<"del buildingpanel"<<std::endl;
}

FactoryPanel::FactoryPanel(InterfaceManager* newui, WagonFactory* f) : 
		BuildingPanel(newui, f),
		factory(f)
{
	SDL_Rect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new WagonTable(this, tablerect, *f));
}

void FactoryPanel::render(Rendering* r)
{
	BuildingPanel::render(r);
	SDL_Rect queuerect = {rect.x + margin_x, 
						  rect.y + getlocalrect().h - margin_y - 20, 
						  getlocalrect().w-2*margin_x, 
						  20};
	std::vector<WagonInfo> wagoninfos;
	for(const WagonType* type : factory->getqueue()){
		WagonInfo info(type->iconname, none, 0);
		wagoninfos.push_back(info);
	}
	rendertrainicons(r, *ui, wagoninfos, queuerect); // TODO: This should be delegated to a queue decoration class accessing the waiting objects, but that requires abstracting the queue into its own class. We'll do this when needed.
}

FactoryPanel::~FactoryPanel()
{
	std::cout<<"del factorypanel"<<std::endl;
}

} // namespace UI