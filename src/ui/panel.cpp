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
	if(layout)
		layout->setpadding({0,0});
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
		create<MainInfoTable>()
		)
	);
	applylayout();
}

MainPanel::~MainPanel()
{}


RouteListPanel::RouteListPanel(InterfaceManager* newui) : 
	Panel(newui)
{
	setlayout(
		create<VBox>(
			create<Close>(),
			create<RouteTable>()
		)
	);
	applylayout();
	placewherefree();
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
	routepanelref->deletereference();
	routepanelref->set(new RoutePanel(ui, routeindex));
}


RoutePanel::RoutePanel(InterfaceManager* newui, int routeid) :
	Panel(newui)
{
    RouteManager& routing = game->getgamestate().getrouting();
    route = routing.getroute(routeid);
	
	rect.h = ui->getuirendering().getuiviewsize().y;
	setlayout(
		create<VBox>(
			create<Close>(),
			create<Routing::AddTurn>(route),
			create<Routing::AddLoadResource>(route),
			create<Routing::AddCouple>(route),
			create<Routing::AddDecouple>(route),
			create<Routing::RemoveOrder>(route),
			create<EditableText>(route->name),
			create<OrderTable>(route)
		)
	);
	applylayout({0,rect.h});
	rect.x = ui->getuirendering().getuiviewsize().x-rect.w;
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
	setlayout(create<VBox>(
		create<Close>(),
		create<TrainTable>()
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
		create<EditableText>(train.name),
		
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
					create<TrainInfoTable>(train),
					create<TrainOrderTable>(train)
				),
				create<TrainCoupler>(train)
			)
		)
	)
	);
	applylayout();
	placewherefree();
}

TrainPanel::~TrainPanel()
{}

BuildingConstructionPanel::BuildingConstructionPanel(InterfaceManager* newui) : Panel(newui)
{
	setlayout(
	create<VBox>(
		create<Close>(),
		create<ConstructionTable>()
	)
	);
	applylayout();
	placewherefree();
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
		create<VBox>(
			create<EditableText>(building->name, UIRect{0, 0, 250, 20}),
			create<HBox>(
				create<Close>(),
				create<Text>(building->type.name, UIRect{0, 0, 150, 20})
			)
		)
	);
	applylayout();
	placewherefree();
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
		create<WagonTable>(*f),
		create<WagonQueue>(*f)
	});
	setlayout(newlayout);
	applylayout();
	placewherefree();
}

FactoryPanel::~FactoryPanel()
{}

} // namespace UI