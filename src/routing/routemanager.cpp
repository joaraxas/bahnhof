#include<iostream>
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

Route* RouteManager::getroute(int id)
{
	if(id<=0){
		return nullptr;
	}
	if(id<=routes.size()){
		return routes[id-1].get();
	}
	std::cout<<"Error: attempted to access route with id "<<id<<" but there are only "<<routes.size()<<" routes"<<std::endl;
	return routes[id-1].get();
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
		id++;
		routeids.push_back(id);
	}
	return routeids;
}