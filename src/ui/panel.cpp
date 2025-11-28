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
	layout->setpadding({8,8});
	return layout;
}

Layout* Panel::getlayout()
{
	return layout;
}

void Panel::applylayout(UIVec minsize=UIVec{10,10})
{
	if(layout){
		UIRect layoutrect = layout->place({0,0,minsize.x,minsize.y});
		rect.w = layoutrect.w;
		rect.h = layoutrect.h;
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
		0, 0, 100, 50
	};
	setlayout(
		create<VBox>(
			create<Close>(),
			create<RouteTable>(tablerect)
		)
	);
	applylayout({rect.w,rect.h});
	routepanelref = std::make_unique<Ownership>();
}

RouteListPanel::~RouteListPanel()
{
	// This prevents calling RoutePanel::erase() in case the RouteListPanel was deleted for an unexpected reason, 
	// like closing the game window. Calling RoutePanel::erase() will cause a segfault as it needs the InputManager
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
    UIRect routepanelrect = {getglobalrect().x-200,0,200,getglobalrect().h};
	routepanelref->deletereference();
	routepanelref->set(new RoutePanel(ui, routepanelrect, routeindex));
}


RoutePanel::RoutePanel(InterfaceManager* newui, UIRect newrect, int routeid) :
	Panel(newui, newrect)
{
    RouteManager& routing = game->getgamestate().getrouting();
    route = routing.getroute(routeid);
	UIRect routenamerect = {0, 0, 100, 14};
	UIRect tablerect = {0, 0, 100, 100};
	setlayout(
		create<VBox>(
			create<Close>(),
			create<Routing::AddTurn>(route),
			create<Routing::AddLoadResource>(route),
			create<Routing::AddCouple>(route),
			create<Routing::AddDecouple>(route),
			create<Routing::RemoveOrder>(route),
			create<EditableText>(route->name, routenamerect),
			create<OrderTable>(tablerect, route)
		)
	);
	applylayout({rect.w,rect.h});
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
    UIRect tablerect = {0, 0, 300, 100};
	setlayout(create<VBox>(
		create<Close>(),
		create<TrainTable>(tablerect)
	));
	applylayout();
    UIVec viewsize = ui->getuirendering().getuiviewsize();
	rect = {viewsize.x*0.5-rect.w*0.5,viewsize.y-rect.h,rect.w,rect.h};
}

TrainListPanel::~TrainListPanel()
{}


TrainPanel::TrainPanel(InterfaceManager* newui, UIRect newrect, TrainManager& manager, Train& newtrain) : 
		Panel(newui, newrect), 
		trainmanager(manager), 
		train(newtrain)
{
	setlayout(
	create<VBox>(
		create<EditableText>(train.name, UIRect{0, 0, 300, 20}),
		
		create<HBox>(
			create<VBox>(
				create<Close>(),
				create<SetRoute>(),
				create<GoTrain>(),
				create<GasTrain>(),
				create<BrakeTrain>(),
				create<TurnTrain>(),
				create<CoupleTrain>()
			),

			create<VBox>(
				create<HBox>(
					create<TrainInfoTable>(UIRect{0, 0, 80, 150}, train),
					create<TrainOrderTable>(UIRect{0, 0, 200, 150}, train)
				),
				create<TrainCoupler>(UIRect{0, 0, 200, 30}, train)
			)
		)
	)
	);
	applylayout();
}

TrainPanel::~TrainPanel()
{}

BuildingConstructionPanel::BuildingConstructionPanel(InterfaceManager* newui, UIRect r) : Panel(newui, r)
{
	UIRect tablerect = {0, 0, 200, 100};
	setlayout(
	create<VBox>(
		create<Close>(),
		create<ConstructionTable>(tablerect)
	)
	);
	applylayout();
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
	setlayout(
		create<HBox>(
			create<Close>(),
			create<EditableText>(building->name, UIRect{0, 0, 250, 20}),
			create<Text>(building->type.name, UIRect{0, 0, 80, 30})
		)
	);
	applylayout();
}

BuildingPanel::~BuildingPanel()
{}

FactoryPanel::FactoryPanel(InterfaceManager* newui, WagonFactory* f) : 
		BuildingPanel(newui, f),
		factory(f)
{
	Layout* newlayout = create<VBox>();
	newlayout->addelements({
		getlayout(),
		create<WagonTable>(UIRect{0,0,300,100}, *f),
		create<WagonQueue>(UIRect{0,0,300,20}, *f)
	});
	setlayout(newlayout);
	applylayout();
}

FactoryPanel::~FactoryPanel()
{}

} // namespace UI