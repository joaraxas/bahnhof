#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/resources/storage.h"

std::vector<std::unique_ptr<Storage>> storages;


ResourceManager::ResourceManager(Game* whatgame)
{
	game = whatgame;
	SpriteManager& s = game->getsprites();
	resourcemap[beer] = new Resource(s, beer, "Beer", sprites::beer);
	resourcemap[hops] = new Resource(s, hops, "Hops", sprites::hops);
	resourcemap[barley] = new Resource(s, barley, "Barley", sprites::barley);
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

Resource::Resource(SpriteManager& s, resourcetype newtype, std::string newname, sprites::name spritename)
{
    type = newtype;
	name = newname;
	sprite.setspritesheet(s, spritename);
	sprite.zoomed = false;
}

void Resource::render(Rendering* r, Vec pos)
{
	sprite.render(r, pos);
}

Storage::Storage(Game* whatgame, int x, int y, int w, int h, resourcetype _accepts, resourcetype _provides)
{
	game = whatgame;
	rect = {x, y, w, h};
	accepts = _accepts;
	provides = _provides;
}

void Storage::render(Rendering* r)
{
	ResourceManager& allresources = game->getresources();
	SDL_SetRenderDrawColor(renderer, 127, 0, 0, 255);
	SDL_Rect drawrect = rect;
	r->renderrectangle(&drawrect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	int xoffset = 0;
	int nCols = 0;
	float scale = r->getscale();
	int sep = 20/scale;
	int frameoffset = fmax(1,int(2/scale));
	for(auto resourcepair : storedresources){
		Resource* resource = allresources.get(resourcepair.first);
		int amount = resourcepair.second;
		if(amount*sep*sep<rect.w*rect.h*0.6){
			for(int i=0; i<amount; i++){
				int maxrows = floor(rect.h/sep);
				nCols = floor(i/maxrows);
				int y = frameoffset+rect.y+sep/2+sep*i-nCols*maxrows*sep;
				int x = frameoffset+xoffset+rect.x+sep/2+sep*nCols;
				resource->render(r, Vec(x, y));
			}
			xoffset += (1+nCols)*sep;
		}
		else{
			int y = rect.y+frameoffset+sep/2;
			int x = rect.x+frameoffset+sep/2+xoffset;
			r->rendertext(std::to_string(amount), x-sep/2, y-7/scale);
			int textwidth = 7*(1+(amount>=10)+(amount>=100))/scale;
			resource->render(r, Vec(x+textwidth, y));
			xoffset += textwidth+sep;
		}
	}
}

int Storage::loadstorage(resourcetype resource, int amount)
{
	if(resource!=none){
		if(storedresources.count(resource)){
			amount = fmin(amount, 20-storedresources[resource]);
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