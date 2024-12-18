#include "bahnhof/routing/routing.h"
#include "bahnhof/graphics/rendering.h"


RouteManager::RouteManager(Tracks::Tracksystem* tracks)
{
    tracksystem = tracks;
}

void RouteManager::renderroutes(Rendering* r, int xoffset, int yoffset)
{
	int offset = 0;
	int textheight = 14*r->getlogicalscale();
	for(auto &route : routes){
		r->rendertext("("+std::to_string(offset+1)+") "+route->name, xoffset, yoffset + (offset)*textheight, {0,0,0,0}, false);
		offset++;
	}
	r->rendertext("(r) Create new route", xoffset, yoffset + (offset)*textheight, {0,0,0,0}, false);
}

Route* RouteManager::addroute()
{
	Route* newroute = new Route(tracksystem, "Route "+std::to_string(routes.size()+1));
	routes.emplace_back(newroute);
    return newroute;
}