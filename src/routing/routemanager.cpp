#include "bahnhof/routing/routing.h"
#include "bahnhof/graphics/rendering.h"


RouteManager::RouteManager(Tracks::Tracksystem* tracks)
{
    tracksystem = tracks;
}

void RouteManager::addroute()
{
	Route* newroute = new Route(tracksystem, "Route "+std::to_string(routes.size()+1));
	routes.emplace_back(newroute);
}

void RouteManager::selectroute(int id)
{
	selectedroute = routes[id].get();
}

std::vector<std::string> RouteManager::getroutenames()
{
	std::vector<std::string> routenames;
	for(auto& route : routes){
		routenames.push_back(route->name);
	}
	return routenames;
}

std::vector<int> RouteManager::getrouteids()
{
	std::vector<int> routeids;
	int id = 0;
	for(auto& route : routes){
		routeids.push_back(id);
		id++;
	}
	return routeids;
}