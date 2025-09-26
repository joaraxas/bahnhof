#include<map>
#include "bahnhof/common/shape.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/ui/ui.h"

std::vector<std::unique_ptr<Storage>> storages;


ResourceManager::ResourceManager(Game* whatgame)
{
	game = whatgame;
	SpriteManager& s = game->getsprites();
	resourcemap[beer] = new Resource(s, beer, "Beer", sprites::beer, sprites::barleybgr);
	resourcemap[hops] = new Resource(s, hops, "Hops", sprites::hops, sprites::hopsbgr);
	resourcemap[barley] = new Resource(s, barley, "Barley", sprites::barley, sprites::barleybgr);
}

ResourceManager::~ResourceManager()
{
	for (auto& pair : resourcemap) {
        delete pair.second;
    }
}

Resource* ResourceManager::get(resourcetype type)
{
	if(type == none)
		return nullptr;
	auto it = resourcemap.find(type);
    return (it != resourcemap.end()) ? it->second : nullptr;
}

Resource::Resource(
	SpriteManager& s, 
	resourcetype newtype, 
	std::string newname, 
	sprites::name iconname,
	sprites::name texturename
	)
{
    type = newtype;
	name = newname;
	icon.setspritesheet(s, iconname);
	fillingtexture.setspritesheet(s, texturename);
	game = s.getgame();
}

void Resource::render(Rendering* r, Vec pos)
{
	icon.render(r, pos);
}

void Resource::renderasshape(Rendering* r, const Shape* shape)
{
	fillingtexture.imageangle = shape->getorientation();
	fillingtexture.render(r, shape->mid());
	// shape->renderfilled(r, {0,0,255,255});
}

Storage::Storage(Game* whatgame, int x, int y, int w, int h)
{
	game = whatgame;
	rect = {x, y, w, h};
}

void Storage::render(Rendering* r)
{
	ResourceManager& allresources = game->getresources();
	InterfaceManager& ui = allresources.getgame().getui();
	SDL_SetRenderDrawColor(renderer, 127, 0, 0, 255);
	r->renderrectangle(rect);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	int xoffset = 0;
	int nCols = 0;
	float camscale = r->getcamscale();
	float uiscale = ui.getuirendering().getuiscale();
	int iconwidth = 20*uiscale/camscale;
	int frameoffset = fmax(1,int(2*uiscale/camscale));
	for(auto resourcepair : storedresources){
		Resource* resource = allresources.get(resourcepair.first);
		int amount = resourcepair.second;
		if(amount*iconwidth*iconwidth<rect.w*rect.h*0.6){
			int maxrows = floor(rect.h/iconwidth);
			for(int i=0; i<amount; i++){
				nCols = floor(i/maxrows);
				int y = frameoffset+rect.y+iconwidth/2+iconwidth*i-nCols*maxrows*iconwidth;
				int x = frameoffset+xoffset+rect.x+iconwidth/2+iconwidth*nCols;
				resource->render(r, Vec(x, y));
			}
			xoffset += (1+nCols)*iconwidth;
		}
		else{
			int y = rect.y+frameoffset;
			int x = rect.x+frameoffset+xoffset;
			SDL_Rect textrect = r->rendertext(std::to_string(amount), x, y);
			int textwidth = textrect.w/camscale;
			resource->render(r, Vec(x+textwidth+iconwidth/2, y+iconwidth/2));
			xoffset += textwidth+iconwidth;
		}
	}
}

bool Storage::containspoint(Vec pos)
{
	return pos.x>=rect.x && pos.x<=rect.x+rect.w && pos.y>=rect.y && pos.y<=rect.y+rect.h;
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

bool Storage::accepts(resourcetype resource)
{
	return acceptedresources.contains(resource);
}

bool Storage::provides(resourcetype resource)
{
	return providedresources.contains(resource);
}

void Storage::setaccepting(resourcetype resource, bool shouldaccept)
{
	if(shouldaccept)
		acceptedresources.insert(resource);
	else
		acceptedresources.erase(resource);
}

void Storage::setproviding(resourcetype resource, bool shouldprovide)
{
	if(shouldprovide)
		providedresources.insert(resource);
	else
		providedresources.erase(resource);
}

resourcetype Storage::getfirstprovidedresource()
{
	return *providedresources.begin();
}


Storage* getstorageatpoint(Vec pos)
{
	for(auto& storage : storages){
		if(storage->containspoint(pos))
			return storage.get();
	}
	return nullptr;
}