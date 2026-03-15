#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/buttons.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/ui/layouts.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/economy/company.h"

namespace UI{
	
Panel::Panel(InterfaceManager* newui) : 
	Host(newui)
{}

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
			create<Bulldoze>(),
			create<ManageRoutes>(),
			create<ManageTrains>(),
			create<ManageCompany>(),
			create<IncreaseUIScale>(),
			create<DecreaseUIScale>()
		),
		create<MainInfoTable>()
		)
	);
	applylayout();
}

void MainPanel::conformtorect(UIRect confrect)
{
	moveto({0,0});
}


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
	placeautomatically();
}


RoutePanel::RoutePanel(InterfaceManager* newui, Route* editroute) :
	Panel(newui), input(game->getinputmanager()), route(editroute)
{
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
	applylayout();
	conformtorect(ui->getuirendering().getuiview());
}

void RoutePanel::close()
{
    input.resetinput();
	Panel::close();
}

void RoutePanel::conformtorect(UIRect confrect)
{
	rect.x = confrect.x+confrect.w - rect.w;
	rect.y = confrect.y;
	rect.h = confrect.h;
	applylayout({rect.w,rect.h});
}


TrainListPanel::TrainListPanel(InterfaceManager* newui) : Panel(newui)
{
	setlayout(create<VBox>(
		create<Close>(),
		create<TrainTable>()
	));
	applylayout();
	placeautomatically();
}


TrainPanel::TrainPanel(InterfaceManager* newui, TrainManager& manager, Train& newtrain) :
		Panel(newui), 
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
	placeautomatically();
}

BuildingConstructionPanel::BuildingConstructionPanel(
	InterfaceManager* newui, const BuildingManager& manager,
    const std::vector<BuildingID>& availabletypes) :
		Panel(newui)
{
	setlayout(
	create<VBox>(
		create<Close>(),
		create<ConstructionTable>(manager, availabletypes)
	)
	);
	applylayout();
	placeautomatically();
}

BuildingPanel::BuildingPanel(
	InterfaceManager* newui, Building& building, std::string& name) : 
		Panel(newui)
{
	setlayout(
		create<VBox>(
			create<EditableText>(name, UIRect{0, 0, 250, 20}),
			create<HBox>(
				create<Close>(),
				create<Text>(building.type.name, UIRect{0, 0, 150, 20})
			),
			create<BuildingOwnerText>(building),
			create<Trade>(building)
		)
	);
	applylayout();
	placeautomatically();
}

BuildingPanel::~BuildingPanel()
{}

FactoryPanel::FactoryPanel(
	InterfaceManager* newui, WagonFactory& factory, std::string& name) : 
		BuildingPanel(newui, factory, name)
{
	getlayout()->setpadding({0,0});
	Layout* newlayout = create<VBox>();
	newlayout->addelements({
		getlayout(),
		create<WagonTable>(factory),
		create<WagonQueue>(factory)
	});
	setlayout(newlayout);
	applylayout();
	placeautomatically();
}

FactoryPanel::~FactoryPanel()
{}

namespace EconomyPanels
{
using namespace Economy;

CompanyPanel::CompanyPanel(InterfaceManager* newui, 
						   Stock& s,
						   std::string& companyname,
						   Portfolio& portfolio,
						   Account& account,
						   Control<Building>& buildings) : 
		Panel(newui),
		stock(s)
{
	setlayout(
	create<VBox>(
		create<EditableText>(companyname),
		create<HBox>(
			create<VBox>(
				create<CompanyInfoTable>(stock, account),
				create<Buy>(stock),
				create<Sell>(stock),
				create<PublicOffering>(stock),
				create<VisitStockmarket>()
			),
			create<VBox>(
				create<Text>("Major owners"),
				create<OwnersTable>(stock)
			),
			create<VBox>(
				create<Text>("Company interests"),
				create<InvestmentsTable>(portfolio),
				create<Close>(),
				create<ListBuildings>(buildings)
			)
		)
	)
	);
	applylayout();
	placeautomatically();
}


InvestorPanel::InvestorPanel(InterfaceManager* newui,
	const std::string& name, Portfolio& p, Account& account) :
	Panel(newui), portfolio(p)
{
	setlayout(
	create<VBox>(
		create<Text>(name),
		create<HBox>(
			create<AccountInfoTable>(account),
			create<VBox>(
				create<Text>("Interests"),
				create<InvestmentsTable>(portfolio)
			)
		),
		create<Close>()
	)
	);
	applylayout();
	placeautomatically();
}

ThePublicPanel::ThePublicPanel(InterfaceManager* newui,
	const std::string& name, Portfolio& p) :
	Panel(newui), portfolio(p)
{
	setlayout(
	create<VBox>(
		create<Text>(name),
		create<VBox>(
			create<Text>("Interests"),
			create<InvestmentsTable>(portfolio)
		),
		create<Close>()
	)
	);
	applylayout();
	placeautomatically();
}

StockmarketPanel::StockmarketPanel(InterfaceManager* newui, 
        const std::vector<Stock*>& stocks) :
	Panel(newui)
{
	setlayout(
	create<VBox>(
		create<Text>("Stock market"),
		create<VBox>(
			create<StocksTable>(stocks)
		),
		create<Close>()
	)
	);
	applylayout();
	placeautomatically();
}

template<>
PossessionsPanel<Building>::PossessionsPanel(InterfaceManager* newui, 
        const std::vector<Building*>& poss) :
	Panel(newui)
{
	setlayout(
	create<VBox>(
		create<Text>("Buildings"),
		create<VBox>(
			create<PossessionsTable<Building>>(poss)
		),
		create<Close>()
	)
	);
	applylayout();
	placeautomatically();
}

} // end namespace EconomyPanels

} // namespace UI