#include "bahnhof/input/input.h"
#include "bahnhof/input/inputmodes.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/track/track.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/ui/ui.h"


IdleMode::IdleMode(Game& g) :
    trainmanager(g.getgamestate().gettrainmanager()),
    buildingmanager(g.getgamestate().getbuildingmanager()),
    tracksystem(g.getgamestate().gettracksystems())
{}

void IdleMode::leftclickmap(Vec mappos)
{
    if(buildingmanager.leftclick(mappos))
        return;
    
    Train* clickedtrain = trainmanager.gettrainatpos(mappos);
    if(clickedtrain){
        selecttrain(clickedtrain);
        return;
    }
    
    if(Tracks::Input::switchat(tracksystem, mappos)){
        return;
    }

    selecttrain(nullptr);
}

void IdleMode::selecttrain(Train* train)
{
    trainmanager.deselectall();
	if(train){
		train->select();
    }
}


DeleteMode::DeleteMode(Game& g) :
    input(g.getinputmanager()),
    ui(g.getui()),
    buildingmanager(g.getgamestate().getbuildingmanager()),
    tracksystem(g.getgamestate().gettracksystems())
{}

void DeleteMode::leftclickmap(Vec mappos)
{
    Tracks::Input::deleteat(tracksystem, mappos);
}

void DeleteMode::rightclickmap(Vec mappos)
{
    input.resetinput();
}

void DeleteMode::render(Rendering* r)
{
    ui.settooltip("Bulldozing");
}


RouteMode::RouteMode(Game& g, Route& route) :
    input(g.getinputmanager()),
    routemanager(g.getgamestate().getrouting()),
    tracksystem(g.getgamestate().gettracksystems()),
    ui(g.getui())
{
    editroute(route);
}

void RouteMode::render(Rendering* r)
{
    editingroute->render(r);
    ui.settooltip("Editing route");
}

void RouteMode::leftclickmap(Vec mappos)
{
    Order* neworder = Tracks::Input::generateorderat(tracksystem, mappos);
    if(neworder)
        editingroute->insertorderatselected(neworder);
}

void RouteMode::rightclickmap(Vec mappos)
{

}

void RouteMode::editroute(Route& route)
{
  	routepanel.set(new UI::RoutePanel(&ui, &route));
    editingroute = &route;
}
