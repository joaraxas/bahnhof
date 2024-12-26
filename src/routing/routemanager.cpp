#include "bahnhof/routing/routing.h"
#include "bahnhof/graphics/rendering.h"


RouteManager::RouteManager(Tracks::Tracksystem* tracks)
{
    tracksystem = tracks;
}

Route* RouteManager::addroute()
{
	Route* newroute = new Route(tracksystem, "Route "+std::to_string(routes.size()+1));
	routes.emplace_back(newroute);
    return newroute;
}

std::vector<std::string> RouteManager::getroutenames()
{
	std::vector<std::string> routenames;
	for(auto& route : routes){
		routenames.push_back(route->name);
	}
	return routenames;
}