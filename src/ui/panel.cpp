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

Layout* Panel::getlayout()
{
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
	UIRect routenamerect = {0, 0, getlocalrect().w, 14};
	UIRect tablerect = {0, 0, getlocalrect().w, getlocalrect().h-300};
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
	applylayout();
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
		addelement(new EditableText(this, train.name, {0, 0, 300, 20})),
		
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
				create<TrainIcons>(UIRect{0, 0, 200, 30}, train)
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
	newlayout->addelement({
		getlayout(),
		create<WagonTable>(UIRect{0,0,300,100}, *f)
	});
	setlayout(newlayout);
	applylayout();
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