#include "bahnhof/routing/routing.h"
#include "bahnhof/graphics/rendering.h"


RouteManager::RouteManager(Tracks::Tracksystem* tracks)
{
    tracksystem = tracks;
}

void RouteManager::renderroutes(Rendering* r, int xoffset, int yoffset)
{
	std::vector<std::string> routenames;
	int iroute = 1;
	for(auto& route : routes){
		routenames.push_back("("+std::to_string(iroute)+") "+route->name);
		iroute++;
	}
	int tableheight = r->rendertable(routenames, {xoffset, yoffset, 200, 1000});
	r->rendertext("(r) Create new route", xoffset, yoffset + tableheight, {0,0,0,0}, false);
}

Route* RouteManager::addroute()
{
	Route* newroute = new Route(tracksystem, "Route "+std::to_string(routes.size()+1));
	routes.emplace_back(newroute);
    return newroute;
}