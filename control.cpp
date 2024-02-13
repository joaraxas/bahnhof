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
	switch(order->order){
		case gotostate:{
			Gotostate* specification = dynamic_cast<Gotostate*>(order);
			done = checkifreachedstate(specification->state);
			if(!done){
				if(tracksystem->isred(wagons.front()->state, (2*gasisforward-1)*(2*wagons.front()->alignedforward-1)))
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
			tracksystem->setswitch(specification->node, specification->nodestate); 
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
	return done;
}

void Train::proceed()
{
	orderid = route->nextorder(orderid);
	Order* order = route->getorder(orderid);
}

void Train::render()
{
	if(selected){
		if(route){
			int iOrder = route->getindex(orderid);
			int rectw = 10;
			SDL_Rect rect = {SCREEN_WIDTH-300-rectw-2,(iOrder+1)*14+2,rectw,rectw};
			SDL_RenderDrawRect(renderer, &rect);
		}
		else
			rendertext("No route assigned", SCREEN_WIDTH-300, (0+1)*14, {0,0,0,0});
	}
}

void Train::checkcollision(int ms, Train* train)
{
	if(size(wagons) >= 1)
	if(size(train->wagons) >= 1){
		float pixels = ms*0.001*speed;
		if(speed>=0){
			wagons.front()->travel(pixels);
			if(norm(wagons.front()->getpos(true) - train->wagons.back()->getpos(false)) < abs(pixels))
				couple(*train, false, true);
			else if(norm(wagons.front()->getpos(true) - train->wagons.front()->getpos(true)) < abs(pixels))
				couple(*train, false, false);
			wagons.front()->travel(-pixels);
		}
		else{
			wagons.back()->travel(pixels);
			if(norm(wagons.back()->getpos(false) - train->wagons.back()->getpos(false)) < abs(pixels))
				couple(*train, true, true);
			else if(norm(wagons.back()->getpos(false) - train->wagons.front()->getpos(true)) < abs(pixels))
				couple(*train, true, false);
			wagons.back()->travel(-pixels);
		}
	}
}

bool Train::checkifreachedstate(State goalstate)
{
	if(norm(wagons.back()->getpos(false) - tracksystem->getpos(goalstate)) < abs(10))
		return true;
	if(norm(wagons.front()->getpos(true) - tracksystem->getpos(goalstate)) < abs(10))
		return true;
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
	speed = 0;
	train.speed = 0;
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

Route::Route(std::string routename){name = routename;}

int Route::getindex(int orderid)
{
	auto it = find(orderids.begin(), orderids.end(), orderid) - orderids.begin();
	if(it<orderids.size()){
		return it;}
	else{
		return 0;}
}

Order* Route::getorder(int orderid)
{
	int orderindex = getindex(orderid);
	return orders[orderindex].get();
}

int Route::nextorder(int orderid)
{
	int orderindex = getindex(orderid);
	if(std::find(orderids.begin(), orderids.end(), orderid) != orderids.end()) // if the last order still exists
		orderindex++;
	if(orderindex>=orderids.size()) orderindex = 0;
	return orderids[orderindex];
}

int Route::appendorder(Order* order)
{
	int neworderid = ordercounter;
	orders.emplace_back(order);
	orderids.push_back(neworderid);
	ordercounter++;
	return(neworderid);
}

void Route::removeorder(int orderid)
{
	int orderindex = getindex(orderid);
	removeorders(orderindex, orderindex);
}

void Route::removeordersupto(int orderid)
{
	int orderindex = getindex(orderid);
	removeorders(0, orderindex);
}

void Route::removeorders(int orderindexfrom, int orderindexto)
{
	orders.erase(orders.begin() + orderindexfrom, orders.begin() + orderindexto + 1);
	orderids.erase(orderids.begin() + orderindexfrom, orderids.begin() + orderindexto + 1);
}

void Route::render()
{
	for(int iOrder=0; iOrder<orderids.size(); iOrder++){
		rendertext("(" + std::to_string(orderids[iOrder]) + ") " + orders[iOrder]->description, SCREEN_WIDTH-300, (iOrder+1)*14, {0,0,0,0});
	}
}

Gotostate::Gotostate(State whichstate)
{
	order = gotostate;
	state = whichstate;
	pass = false;
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

Setswitch::Setswitch(nodeid whichnode, bool upordown, int whichnodestate)
{
	order = o_setswitch;
    node = whichnode;
    updown = upordown;
	nodestate = whichnodestate;
	flip = false;
	if(nodestate == -1){
		flip = true;
		description = "Flip switch " + std::to_string(node);
	}
	else
		description = "Set switch " + std::to_string(node) + " to state " + std::to_string(nodestate);
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