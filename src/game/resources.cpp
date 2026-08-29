#include<map>
#include "bahnhof/common/shape.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/resources.h"


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
