#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
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
		if(keys[gearbutton]){
			if(speed==0){
				gasisforward = 1-gasisforward;
				speed = 5*(2*gasisforward-1);
			}
		}
		for(int iKey=1; iKey<fmin(wagons.size(), sizeof(numberbuttons)/sizeof(*numberbuttons)); iKey++)
			if(keys[numberbuttons[iKey]]) split(iKey);
	}
		if(keys[loadbutton]||1==1){
			if(speed==0)
			for(auto w : wagons){
				Storage* storage = getstorageatpoint(w->pos);
				if(storage){
					int unloadedamount = storage->unloadstorage(storage->provides, 1);
					int loadedamount = w->loadwagon(storage->provides, unloadedamount);
					if(loadedamount!=unloadedamount)
						storage->loadstorage(storage->provides, unloadedamount-loadedamount);
				}
			}
		}
		if(keys[unloadbutton]||1==1){
			if(speed==0)
			for(auto w : wagons){
				Storage* storage = getstorageatpoint(w->pos);
				if(storage){
					resourcetype beingunloaded;
					int unloadedamount = w->unloadwagon(&beingunloaded);
					if(storage->accepts==beingunloaded)
						storage->loadstorage(beingunloaded, unloadedamount);
					else
						w->loadwagon(beingunloaded, unloadedamount);
				}
			}
		}
	//}
}

void Train::update(int ms)
{
	float pixels = ms*0.001*speed;
	if(tracksystem->isred(wagons.front()->state, (2*gasisforward-1)*(2*wagons.front()->alignedforward-1)))
		brake(ms);
	else if(!selected)
		gas(ms);
	for(auto& wagon : wagons)
		wagon->travel(pixels);
}

void Train::checkCollision(int ms, Train* train)
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

void Train::gas(int ms)
{
	float Ptot = 0;
	for(auto w : wagons)
		Ptot += w->getpower();
	float mtot = size(wagons);
	speed+=(2*gasisforward-1)*ms*Ptot/mtot;
}

void Train::brake(int ms)
{
	float Ptot = 0;
	for(auto w : wagons)
		Ptot += 0.2;
	float mtot = size(wagons);
	speed = (2*gasisforward-1)*fmax(0,(2*gasisforward-1)*(speed - (2*gasisforward-1)*ms*Ptot/mtot));
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

void Train::split(int where)
{
	if(speed==0)
	if(wagons.size()>where){
		trains.emplace_back(new Train(*tracksystem, {wagons.begin() + where, wagons.end()}, speed));
		wagons = {wagons.begin(), wagons.begin() + where};
		std::cout << "split" << std::endl;
	}
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