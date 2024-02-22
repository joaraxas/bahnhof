#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "utils.h"

std::vector<std::unique_ptr<Train>> trains;
std::vector<std::unique_ptr<Storage>> storages;


Train::Train(Tracksystem& newtracksystem, const std::vector<Wagon*> &newwagons, float newspeed)
{
	tracksystem = &newtracksystem;
	wagons = newwagons;
	speed = newspeed;
	for(auto wagon : wagons)
		wagon->train = this;
}

void Train::getinput(int ms)
{
	if(selected){
		if(keys[gasbutton])
			gas(ms);
		if(keys[brakebutton])
			brake(ms);
		if(keys[gearbutton])
			shiftdirection();
		for(int iKey=1; iKey<fmin(wagons.size(), sizeof(numberbuttons)/sizeof(*numberbuttons)); iKey++)
			if(keys[numberbuttons[iKey]]) split(iKey);
		if(keys[loadbutton])
			loadall();
		if(keys[unloadbutton])
			unloadall();
	}
}

void Train::update(int ms)
{
	if(go)
		if(perform(ms))
			proceed();
	float pixels = ms*0.001*speed;
	for(auto& wagon : wagons)
		wagon->travel(pixels);
}

bool Train::perform(int ms)
{
	bool done = false;
	if(route){
	Order* order = route->getorder(orderid);
	if(!order){//orderid does not exist in route
	proceed();
	//if(orderid==-1)//route has no orders
	//brake(ms);
	}
	else{
	switch(order->order){
		case gotostate:{
			Gotostate* specification = dynamic_cast<Gotostate*>(order);
			done = checkifreachedstate(specification->state, ms);
			if(!done){
				if(tracksystem->isred(forwardstate()))
					brake(ms);
				else
					gas(ms);}
			break;}
		//case 1:
		//	done = checkifleftstate(state); break;
		case o_setsignal:{
			Setsignal* specification = dynamic_cast<Setsignal*>(order);
			tracksystem->setsignal(specification->signal, specification->redgreenflip);
			done = true;
			break;}
		case o_setswitch:{
			Setswitch* specification = dynamic_cast<Setswitch*>(order);
			tracksystem->setswitch(specification->node, specification->updown, specification->nodestate); 
			done = true;
			break;}
		//case couple:
		//	done = checkifcoupled(); break
		case decouple:{
			Decouple* specification = dynamic_cast<Decouple*>(order);
			done = split(specification->where, specification->route);
			if(!done)
				brake(ms);
			break;}
		case turn:{
			done = shiftdirection();
			if(!done)
				brake(ms);
			break;}
		case loadresource:{
			Loadresource* specification = dynamic_cast<Loadresource*>(order);
			if(specification->anyresource){
				bool doneloading = true; bool doneunloading = true;
				if(specification->loading)
					doneloading = loadall();
				if(specification->unloading)
					doneunloading = unloadall();
				done = doneloading && doneunloading;
				if(!done)
					brake(ms);
			}
			break;}
		case wipe:{
			route->removeordersupto(orderid);
			done = true;
			break;}
	}
	}
	}
	return done;
}

void Train::proceed()
{
	orderid = route->nextorder(orderid);
}

void Train::render()
{
	if(selected){
		if(route){
			if(orderid>=0){
				int iOrder = route->getindex(orderid);
				int rectw = 10;
				SDL_Rect rect = {SCREEN_WIDTH-300-rectw-2,(iOrder+1)*14+2,rectw,rectw};
				SDL_SetRenderDrawColor(renderer, 200*(!go), 63*go, 0, 255);
				SDL_RenderDrawRect(renderer, &rect);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
		}
		else
			rendertext("No route assigned", SCREEN_WIDTH-300, (0+1)*14, {0,0,0,0});
		if(!nicetracks){
			Vec frontpos = tracksystem->getpos(forwardstate());
			rendertext("for", frontpos.x, frontpos.y, {0,255,0,255});
			Vec backpos = tracksystem->getpos(backwardstate());
			rendertext("bac", backpos.x, backpos.y, {255,0,0,255});
		}
	}
}

State Train::forwardstate()
{
	if(gasisforward){
		if(wagons.front()->alignedforward)
			return wagons.front()->frontendstate();
		else
			return flipstate(wagons.front()->backendstate());
	}
	else{
		if(wagons.back()->alignedforward)
			return flipstate(wagons.back()->backendstate());
		else
			return wagons.back()->frontendstate();
	}
}

State Train::backwardstate()
{
	if(gasisforward){
		if(wagons.back()->alignedforward)
			return flipstate(wagons.back()->backendstate());
		else
			return wagons.back()->frontendstate();
	}
	else{
		if(wagons.front()->alignedforward)
			return wagons.front()->frontendstate();
		else
			return flipstate(wagons.front()->backendstate());
	}
}

void Train::checkcollision(int ms, Train* train)
{
	if(wantstocouple || train->wantstocouple){
		if(size(wagons) >= 1)
		if(size(train->wagons) >= 1){
			float pixels = ms*0.001*speed;/*
			if(speed>=0){
				wagons.front()->travel(pixels);
				if(norm(wagons.front()->frontendstate() - train->wagons.back()->backendstate()) < abs(pixels))
					couple(*train, false, true);
				else if(norm(wagons.front()->frontendstate() - train->wagons.front()->frontendstate()) < abs(pixels))
					couple(*train, false, false);
				wagons.front()->travel(-pixels);
			}
			else{
				wagons.back()->travel(pixels);
				if(norm(wagons.back()->backendstate() - train->wagons.back()->backendstate()) < abs(pixels))
					couple(*train, true, true);
				else if(norm(wagons.back()->backendstate() - train->wagons.front()->frontendstate()) < abs(pixels))
					couple(*train, true, false);
				wagons.back()->travel(-pixels);
			}*/
		}
	}
}

bool Train::checkifreachedstate(State goalstate, int ms)
{
	float pixels = 4*abs(speed)*ms*0.001;
	if(tracksystem->distancefromto(flipstate(forwardstate()), goalstate, pixels)<pixels)
		return true;
	else
		return false;
}

bool Train::gas(int ms)
{
	float Ptot = 0;
	for(auto w : wagons)
		Ptot += w->getpower();
	float mtot = size(wagons);
	speed+=(2*gasisforward-1)*ms*Ptot/mtot;
	return true;
}

bool Train::brake(int ms)
{
	float Ptot = 0;
	for(auto w : wagons)
		Ptot += 0.2;
	float mtot = size(wagons);
	speed = (2*gasisforward-1)*fmax(0,(2*gasisforward-1)*(speed - (2*gasisforward-1)*ms*Ptot/mtot));
	return (speed==0);
}

bool Train::shiftdirection()
{
	if(speed==0){
		gasisforward = 1-gasisforward;
		speed = 5*(2*gasisforward-1);
		return true;
	}
	else
		return false;
}

bool Train::loadall()
{
	bool done = true;
	if(speed!=0)
		done = false;
	else for(auto w : wagons){
		Storage* storage = getstorageatpoint(w->pos);
		if(storage){
			int unloadedamount = storage->unloadstorage(storage->provides, 1);
			int loadedamount = w->loadwagon(storage->provides, unloadedamount);
			if(loadedamount!=unloadedamount)
				storage->loadstorage(storage->provides, unloadedamount-loadedamount);
			if(loadedamount!=0)
				done = false;
		}
	}
	return done;
}

bool Train::unloadall()
{
	bool done = true;
	if(speed!=0)
		done = false;
	else for(auto w : wagons){
		Storage* storage = getstorageatpoint(w->pos);
		if(storage){
			resourcetype beingunloaded;
			int unloadedamount = w->unloadwagon(&beingunloaded);
			if(storage->accepts==beingunloaded){
				storage->loadstorage(beingunloaded, unloadedamount);
				done = false;
			}
			else
				w->loadwagon(beingunloaded, unloadedamount);
		}
	}
	return done;
}

void Train::couple(Train& train, bool ismyback, bool ishisback)
{
	speed = 0;
	train.speed = 0;
	go = false;
	train.go = false;
	if(wantstocouple || train.wantstocouple){
		bool flipdirection = false;
		if(ismyback && !ishisback){
			std::cout << "couple back front" << std::endl;
			wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
		}
		else if(ismyback && ishisback){
			std::cout << "couple back back" << std::endl;
			wagons.insert(wagons.end(), train.wagons.rbegin(), train.wagons.rend());
			flipdirection = 1;
		}
		else if(!ismyback && !ishisback){
			std::cout << "couple front front" << std::endl;
			std::reverse(wagons.begin(), wagons.end());
			wagons.insert(wagons.end(), train.wagons.begin(), train.wagons.end());
			std::reverse(wagons.begin(), wagons.end());
			flipdirection = 1;
		}
		else if(!ismyback && ishisback){
			std::cout << "couple front back" << std::endl;
			std::reverse(wagons.begin(), wagons.end());
			wagons.insert(wagons.end(), train.wagons.rbegin(), train.wagons.rend());
			std::reverse(wagons.begin(), wagons.end());
		}
		if(flipdirection)
			for(auto w : train.wagons){
				w->alignedforward = 1 - w->alignedforward;
			}
		train.wagons = {};
		for(auto wagon : wagons)
			wagon->train = this;
		if(train.selected)
			selected = true;
	}
}

bool Train::split(int where, Route* assignedroute)
{
	bool splitsucceed = true;
	if(speed!=0)
		splitsucceed = false;
	else if(wagons.size()>where){
		Train* newtrain = new Train(*tracksystem, {wagons.begin() + where, wagons.end()}, speed);
		trains.emplace_back(newtrain);
		newtrain->route = assignedroute;
		wagons = {wagons.begin(), wagons.begin() + where};
		std::cout << "split" << std::endl;
	}
	return splitsucceed;
}

Route::Route(Tracksystem* whattracksystem, std::string routename)
{
	tracksystem = whattracksystem;
	name = routename;
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
    updowns.clear();
	for(auto& order: orders)
		order->assignroute(this);
}

void Route::render()
{
	if(orderids.empty())
		rendertext("Route has no orders yet", SCREEN_WIDTH-300, 1*14, {0,0,0,0});
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
			rendertext("(" + std::to_string(renderordernr) + ") " + orders[iOrder]->description, x, y, {intsty,intsty,intsty,0});
			orders[iOrder]->render(renderordernr);
		}
	}
}

void Order::assignroute(Route* newroute)
{
	route = newroute;
}

Gotostate::Gotostate(State whichstate, bool mustpass)
{
	order = gotostate;
	state = whichstate;
	pass = mustpass;
	description = "Reach state at track " + std::to_string(state.track) + " and nodedist " + std::to_string(state.nodedist);
}

Setsignal::Setsignal(signalid whichsignal, int redgreenorflip)
{
	order = o_setsignal;
    signal = whichsignal;
    redgreenflip = redgreenorflip;
	if(redgreenflip==0)
		description = "Set signal " + std::to_string(signal) + " to red";
	else if(redgreenflip==1)
		description = "Set signal " + std::to_string(signal) + " to green";
	else if(redgreenflip==2)
		description = "Flip signal " + std::to_string(signal);
}

void Setsignal::assignroute(Route* newroute)
{
	offset = 0;
	Order::assignroute(newroute);
	for(int iSignal=0; iSignal<route->signals.size(); iSignal++)
		if(route->signals[iSignal]==signal)
			offset++;
	newroute->signals.push_back(signal);
}

Setswitch::Setswitch(nodeid whichnode, bool upordown, int whichnodestate)
{
	order = o_setswitch;
    node = whichnode;
    updown = upordown;
	nodestate = whichnodestate;
	flip = false;
	std::string switchname = "switch " + std::to_string(node);
	if(updown) switchname+= " up"; else switchname+= " down";
	if(nodestate == -1){
		flip = true;
		description = "Flip " + switchname;
	}
	else{
		if(nodestate==0)
			description = "Set " + switchname + " to left";
		else if(nodestate==1)
			description = "Set " + switchname + " to right";
		else
			description = "Set " + switchname + " to track no. " + std::to_string(nodestate+1) + " counting from left";
	}
}

void Setswitch::assignroute(Route* newroute)
{
	offset = 0;
	Order::assignroute(newroute);
	for(int iSwitch=0; iSwitch<route->switches.size(); iSwitch++)
		if(route->switches[iSwitch]==node && route->updowns[iSwitch]==updown)
			offset++;
	newroute->switches.push_back(node);
	newroute->updowns.push_back(updown);
}

Decouple::Decouple(int keephowmany, Route* givewhatroute)
{
	order = decouple;
	where = keephowmany;
	route = givewhatroute;
	description = "Decouple";
	if(where>1)
		description += " " + std::to_string(where) + " wagons";
	if(route)
		description += ", assign route " + route->name + " to other half";
}

Turn::Turn()
{
	order = turn;
	description = "Switch travel direction";
}

Loadresource::Loadresource()
{
	order = loadresource;
	resource = none;
	anyresource = true;
	loading = true;
	unloading = true;
	description = "Load and unload all possible cargo";
}

Wipe::Wipe()
{
	order = wipe;
	description = "Wipe all previous orders";
}

void Order::renderlabel(Vec pos, int number, SDL_Color bgrcol, SDL_Color textcol)
{
	int x = int(pos.x); int y = int(pos.y);
	SDL_Rect rect = {x,y,16,14};
	SDL_SetRenderDrawColor(renderer, bgrcol.r, bgrcol.g, bgrcol.b, bgrcol.a);
	SDL_RenderFillRect(renderer, &rect);
	rendertext(std::to_string(number), x+1, y, textcol);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void Gotostate::render(int number)
{
	Vec posleft = route->tracksystem->getpos(state, 12);
	Vec posright = route->tracksystem->getpos(state,-12);
	SDL_RenderDrawLine(renderer, posleft.x, posleft.y, posright.x, posright.y);
	if(posright.x>=posleft.x){
		Order::renderlabel(posright, number);
	}
	else{
		Order::renderlabel(posleft, number);
	}
}

void Setswitch::render(int number)
{
	Vec pos = route->tracksystem->getswitchpos(node, updown);
	Vec lineend = pos+Vec(12+18*offset,-7);
	Vec inlabel = lineend+Vec(0+10,10);
	Order::renderlabel(lineend, number, {0, 0, 0, 255}, {255, 255, 255, 0});
	SDL_RenderDrawLine(renderer, inlabel.x+4-nodestate*4, inlabel.y, inlabel.x+nodestate*4, inlabel.y-8);
}

void Setsignal::render(int number)
{
	Vec pos = route->tracksystem->getsignalpos(signal);
	Vec lineend = pos+Vec(-8,12+16*offset);
	//SDL_RenderDrawLine(renderer, pos.x, pos.y, lineend.x, lineend.y);
	SDL_Color bgrcol = {255, 0, 0, 255};
	if(redgreenflip==1)
		bgrcol = {0, 255, 0, 255};
	else if(redgreenflip==2)
		bgrcol = {255, 255, 0, 255};
	Order::renderlabel(lineend, number, bgrcol, {0, 0, 0, 255});
}

ResourceManager::ResourceManager()
{
	resourcemap[beer] = new Resource(beer, "Beer", "assets/beer.png");
	resourcemap[hops] = new Resource(hops, "Hops", "assets/hops.png");
}

ResourceManager::~ResourceManager()
{
	for (auto& pair : resourcemap) {
        delete pair.second;
    }
}

Resource* ResourceManager::get(resourcetype type)
{
	auto it = resourcemap.find(type);
    return (it != resourcemap.end()) ? it->second : nullptr;
}

Resource::Resource(resourcetype newtype, std::string newname, std::string pathtotex)
{
    type = newtype;
	name = newname;
	tex = loadImage(pathtotex);
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
}

void Resource::render(Vec pos)
{
	int x = int(pos.x);
	int y = int(pos.y);
	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect rect = {int(x - w / 2), int(y - h / 2), int(w), int(h)};
	SDL_RenderCopyEx(renderer, tex, &srcrect, &rect, -0 * 180 / pi, NULL, SDL_FLIP_NONE);
}

Storage::Storage(ResourceManager& resources, int x, int y, int w, int h, resourcetype _accepts, resourcetype _provides)
{
	allresources = &resources;
	rect = {x, y, w, h};
	accepts = _accepts;
	provides = _provides;
}

void Storage::render()
{
	SDL_SetRenderDrawColor(renderer, 127, 0, 0, 255);
	SDL_RenderDrawRect(renderer, &rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	int xoffset = 0;
	int nCols = 0;
	int sep = 20;
	for(auto resourcepair : storedresources){
		Resource* resource = allresources->get(resourcepair.first);
		int amount = resourcepair.second;
		for(int i=0; i<amount; i++){
			int maxrows = floor(rect.h/sep);
			nCols = floor(i/maxrows);
			int y = 5+rect.y+sep/2+sep*i-nCols*maxrows*sep;
			int x = 5+xoffset+rect.x+sep/2+sep*nCols;
			resource->render(Vec(x, y));
		}
		xoffset += (1+nCols)*sep;
	}
}

int Storage::loadstorage(resourcetype resource, int amount)
{
	if(resource!=none){
		if(storedresources.count(resource)){
			storedresources[resource] += amount;
		}
		else
			storedresources[resource] = amount;
	}
	else
		amount = 0;
	return amount;
}

int Storage::unloadstorage(resourcetype resource, int amount)
{
	int unloadedamount = 0;
	if(storedresources.count(resource)){
		unloadedamount = fmin(storedresources[resource], amount);
		storedresources[resource] -= unloadedamount;
		if(storedresources[resource] == 0)
			storedresources.erase(resource);
	}
	return unloadedamount;
}

bool Storage::containspoint(Vec pos)
{
	return pos.x>=rect.x && pos.x<=rect.x+rect.w && pos.y>=rect.y && pos.y<=rect.y+rect.h;
}

resourcetype Storage::getfirststoredresource()
{
	if(storedresources.size()>0)
		return storedresources.begin()->first;
	else
		return none;
}

Storage* getstorageatpoint(Vec pos)
{
	for(auto& storage : storages){
		if(storage->containspoint(pos))
			return storage.get();
	}
	return nullptr;
}