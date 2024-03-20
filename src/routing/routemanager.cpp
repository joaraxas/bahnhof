#include "bahnhof/routing/routing.h"
#include "bahnhof/graphics/rendering.h"

RouteManager::RouteManager(Tracksystem* tracks)
{
    tracksystem = tracks;
}

void RouteManager::renderroutes(Rendering* r)
{
	int offset = 1;
	for(auto &route : routes){
		r->rendertext("("+std::to_string(offset)+") "+route->name, SCREEN_WIDTH-300, (offset+1)*14, {0,0,0,0}, false);
		offset++;
	}
	r->rendertext("(r) Create new route", SCREEN_WIDTH-300, (offset+1)*14, {0,0,0,0}, false);
}

Route* RouteManager::addroute()
{
	Route* newroute = new Route(tracksystem, "Route "+std::to_string(routes.size()+1));
	routes.emplace_back(newroute);
    return newroute;
}