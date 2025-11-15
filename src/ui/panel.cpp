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
	
Panel::Panel(InterfaceManager* newui, UIRect newrect) : 
	Host(newui, newrect)
{}

Panel::Panel(InterfaceManager* newui) : 
	Panel::Panel(newui, {0,0,100,100}) {}

template <class T, typename... Args> 
T* Panel::create(Args&&... args)
{
	T* el = new T(
		this, 
		std::forward<Args>(args)...);
	addelement(el);
	return el;
}

Layout* Panel::setlayout(Layout* l)
{
	layout = l;
	return layout;
}

void Panel::applylayout()
{
	if(layout){
		UIVec sz = layout->consolidate();
		rect.w = sz.x; rect.h = sz.y;
	}
}

void Panel::render(Rendering* r)
{
	ui->getuirendering().renderrectangle(r, getglobalrect(), PanelBackground, true);
	ui->getuirendering().renderrectangle(r, getglobalrect(), PanelBorder, false);
	Host::render(r);
}


MainPanel::MainPanel(InterfaceManager* newui) : Panel(newui)
{
	setlayout(
	create<HBox>(
		create<VBox>(
			create<Close>(),
			create<PlaceTrack>(),
			create<PlaceSignal>(),
			create<PlaceBuildings>(),
			create<ManageRoutes>(),
			create<ManageTrains>(),
			create<IncreaseUIScale>(),
			create<DecreaseUIScale>()
		),
		create<MainInfoTable>(UI::UIRect{0,0,60,100})
	)
	);
	applylayout();
}

MainPanel::~MainPanel()
{}


RouteListPanel::RouteListPanel(InterfaceManager* newui, UIRect newrect) : 
	Panel(newui, newrect)
{
	UIRect tablerect = {
		0, 0, 
		getlocalrect().w, 
		getlocalrect().h-50
	};
	setlayout(
		create<VBox>(
			create<Close>(),
			create<RouteTable>(tablerect)
		)
	);
	applylayout();
	routepanelref = std::make_unique<Ownership>();
}

RouteListPanel::~RouteListPanel()
{
	// This prevents calling RoutePanel::erase() in case the RouteListPanel was deleted for an unexpected reason, 
	// like closing the game window. Calling RoutePanel::erase() will throw a segfault as it needs the InputManager
	// which has already been destroyed at this point.
	routepanelref->resetreference();
}

void RouteListPanel::erase()
{
	// We need to do this here and not in the destructor because this just adds the route panel to the list of
	// hosts to be deleted, and that list is being iterated through when the destructor is called.
	routepanelref->deletereference();
	Panel::erase();
}

void RouteListPanel::addroutepanel(int routeindex)
{
    UIRect routepanelrect = {getlocalrect().x-200,0,200,getlocalrect().h};
	routepanelref->deletereference();
	routepanelref->set(new RoutePanel(ui, routepanelrect, routeindex));
}


RoutePanel::RoutePanel(InterfaceManager* newui, UIRect newrect, int routeid) :
	Panel(newui, newrect)
{
    RouteManager& routing = game->getgamestate().getrouting();
    route = routing.getroute(routeid);
	create<Routing::AddTurn>(route);
	create<Routing::AddLoadResource>(route);
	create<Routing::AddCouple>(route);
	create<Routing::AddDecouple>(route);
	create<Routing::RemoveOrder>(route);
	UIRect routenamerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, 14};
	yoffset += 16;
	UIRect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new OrderTable(this, tablerect, route));
	addelement(new EditableText(this, route->name, routenamerect));
	game->getinputmanager().editroute(route);
}

RoutePanel::~RoutePanel()
{}

void RoutePanel::erase()
{
    game->getinputmanager().editroute(nullptr);
	Panel::erase();
}


TrainListPanel::TrainListPanel(InterfaceManager* newui) : Panel(newui)
{
    UIVec viewsize = ui->getuirendering().screentoui(getviewsize());
    rect = {viewsize.x*0.5-150,viewsize.y-150,300,150};
	UIRect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new TrainTable(this, tablerect));
}

TrainListPanel::~TrainListPanel()
{}


TrainPanel::TrainPanel(InterfaceManager* newui, UIRect newrect, TrainManager& manager, Train& newtrain) : 
		Panel(newui, newrect), 
		trainmanager(manager), 
		train(newtrain)
{
	create<SetRoute>();
	create<GoTrain>();
	create<GasTrain>();
	create<BrakeTrain>();
	create<TurnTrain>();
	create<CoupleTrain>();

	Coord column_2_x = margin_x+80+elementdistance_x;
	Coord columns_y = margin_y+20+elementdistance_y;

	UIRect traininfotablerect = {column_2_x, columns_y, 100, 100};
	addelement(new TrainInfoTable(this, traininfotablerect, train));

	UIRect trainiconsrect = {column_2_x, columns_y+140+elementdistance_y, 200, 30};
	addelement(new TrainIcons(this, trainiconsrect, train));

	Coord column_3_x = column_2_x + 100 + elementdistance_x;
	UIRect routetablerect = {column_3_x, columns_y, rect.w-column_3_x-margin_x, rect.h-columns_y-margin_y-35};
	addelement(new TrainOrderTable(this, routetablerect, train));
	
	UIRect trainnamerect = {column_2_x, margin_y, getlocalrect().w-2*column_2_x, 20};
	addelement(new EditableText(this, train.name, trainnamerect));
}

TrainPanel::~TrainPanel()
{}

BuildingConstructionPanel::BuildingConstructionPanel(InterfaceManager* newui, UIRect r) : Panel(newui, r)
{
	UIRect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-margin_y-yoffset-elementdistance_y};
	addelement(new ConstructionTable(this, tablerect));
}

BuildingConstructionPanel::~BuildingConstructionPanel()
{}

void BuildingConstructionPanel::erase()
{
	game->getinputmanager().resetinput();
	Panel::erase();
}

BuildingPanel::BuildingPanel(InterfaceManager* newui, Building* b) : 
		Panel(newui, {200,400,500,150}),
		building(b)
{
	Coord column_2_x = margin_x + 80 + elementdistance_x;
	Coord typenamewidth = 80;
	Coord column_3_x = rect.w - typenamewidth - margin_x;
	Coord namewidth = column_3_x - column_2_x - elementdistance_x;
	addelement(new EditableText(this, building->name, {column_2_x, 10, namewidth, 20}));
	addelement(new Text(this, building->type.name, {column_3_x, 10, typenamewidth, 20}));
}

BuildingPanel::~BuildingPanel()
{}

FactoryPanel::FactoryPanel(InterfaceManager* newui, WagonFactory* f) : 
		BuildingPanel(newui, f),
		factory(f)
{
	UIRect tablerect = {margin_x, margin_y+yoffset, getlocalrect().w-2*margin_x, getlocalrect().h-2*margin_y-yoffset};
	addelement(new WagonTable(this, tablerect, *f));
}

void FactoryPanel::render(Rendering* r)
{
	BuildingPanel::render(r);
	UIRect queuerect = {rect.x + margin_x, 
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
{}

} // namespace UI