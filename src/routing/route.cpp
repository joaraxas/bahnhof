#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/routing/routing.h"


Route::Route(Tracks::Tracksystem* tracks, std::string routename)
{
	name = routename;
	tracksystem = tracks;
}

int Route::getindex(int orderid)
{
	auto it = find(orderids.begin(), orderids.end(), orderid) - orderids.begin();
	if(it<orderids.size()){
		return it;}
	else{
		return -1;}
}

Order* Route::getorder(int orderid)
{
	int orderindex = getindex(orderid);
	if(orderindex>=0)
		return orders[orderindex].get();
	else
		return nullptr;
}

int Route::previousorder(int orderid)
{
	int orderindex = getindex(orderid);
	if(orderindex>=0){ // if the last order still exists
		orderindex--;
		if(orderindex<0) orderindex = orderids.size()-1;
		return orderids[orderindex];
	}
	else if(orderids.size()>0)
		return orderids[orderids.size()-1];
	else
		return -1;
}

int Route::nextorder(int orderid)
{
	int orderindex = getindex(orderid);
	if(orderindex>=0){ // if the last order still exists
		orderindex++;
		if(orderindex>=orderids.size()) orderindex = 0;
		return orderids[orderindex];
	}
	else if(orderids.size()>0)
		return orderids[0];
	else
		return -1;
}

int Route::appendorder(Order* order)
{
	int neworderid = insertorder(order, orderids.size()-1);
	return(neworderid);
}

int Route::insertorderatselected(Order* order)
{
	int neworderid = insertorder(order, getindex(selectedorderid));
	return(neworderid);
}

int Route::insertorder(Order* order, int orderindex)
{
	int neworderid = ordercounter;
	if(orderindex<0 || orderindex>=orderids.size()) orderindex = orderids.size()-1;
	order->assignroute(this);
	orders.emplace(orders.begin() + orderindex + 1, order);
	orderids.insert(orderids.begin() + orderindex + 1, neworderid);
	ordercounter++;
	selectedorderid = neworderid;
	return(neworderid);
}

void Route::removeselectedorder()
{
	if(selectedorderid>=0){
		int selectedindex = fmin(getindex(selectedorderid), orderids.size()-1-1);
		removeorder(selectedorderid);
		//TODO: remove the below when mouse input implemented for route editing
		selectedorderid = orderids[selectedindex];
	}
}

void Route::removeorder(int orderid)
{
	int orderindex = getindex(orderid);
	if(orderindex>=0)
		removeorders(orderindex, orderindex);
}

void Route::removeordersupto(int orderid)
{
	int orderindex = getindex(orderid);
	if(orderindex>=0)
		removeorders(0, orderindex);
}

void Route::removeorders(int orderindexfrom, int orderindexto)
{
	if(orderindexfrom>=0)
	if(orderindexto>=0)
	if(size(orderids)>0){
		if(orderindexto>orderids.size()-1) orderindexto = orderids.size()-1;
		orders.erase(orders.begin() + orderindexfrom, orders.begin() + orderindexto + 1);
		orderids.erase(orderids.begin() + orderindexfrom, orderids.begin() + orderindexto + 1);
		if(getindex(selectedorderid)<0) selectedorderid = -1;
	}

    signals.clear();
    switches.clear();
	for(auto& order: orders)
		order->assignroute(this);
}

void Route::render(Rendering* r)
{
	if(orderids.empty())
		r->rendertext("Route has no orders yet", SCREEN_WIDTH-300, 1*14, {0,0,0,0}, false);
	else{
		int renderordernr = 1;
		if(orders[0]->order==gotostate)
			renderordernr = 0;
		for(int iOrder=0; iOrder<orderids.size(); iOrder++){
			int oid = orderids[iOrder];
			int x = SCREEN_WIDTH-300;
			int y = (iOrder+1)*14;
			Uint8 intsty = (oid==selectedorderid)*255;
			if(orders[iOrder]->order==gotostate)
				renderordernr++;
			r->rendertext("(" + std::to_string(renderordernr) + ") " + orders[iOrder]->description, x, y, {intsty,intsty,intsty,0}, false);
			orders[iOrder]->render(r, renderordernr);
		}
	}
}