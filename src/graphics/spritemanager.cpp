#include<iostream>
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"


SpriteManager::SpriteManager(Game* g) : game(g)
{
	using namespace sprites;
	addspritesheet(openwagon, "rollingstock/openwagon.png");
	addspritesheet(refrigeratorcar, "rollingstock/refrigeratorcar.png");
	addspritesheet(tankloco, "rollingstock/loco0.png", 4);

	addspritesheet(iconopenwagon, "icons/openwagon.png");
	addspritesheet(iconrefrigeratorcar, "icons/refrigeratorcar.png");
	addspritesheet(icontankloco, "icons/loco.png");

	addspritesheet(switchsprite, "track/switch.png", 1, 2);
	get(switchsprite)->setoriginy(15);
	addspritesheet(sprites::signal, "track/signal.png", 1, 2);
	get(sprites::signal)->setoriginx(3);
	get(sprites::signal)->setoriginy(31);
	addspritesheet(bufferstop, "track/bufferstop.png");
	get(bufferstop)->setoriginx(0);

	addspritesheet(light, "effects/light.png", 1, 2);

	addspritesheet(beer, "resources/icons/beer.png");
	addspritesheet(hops, "resources/icons/hops.png");
	addspritesheet(barley, "resources/icons/barley.png");

	addspritesheet(barleybgr, "resources/textures/barley.png");
	addspritesheet(hopsbgr, "resources/textures/hops.png");

	addspritesheet(brewery, "buildings/brewery2.png");
	addspritesheet(tavern, "buildings/tavern.png");
	addspritesheet(hopsfield, "buildings/hopsfield.png");
	addspritesheet(barleyfield, "buildings/barleyfield.png");
	addspritesheet(wagonfactory, "buildings/wagonfactory.png");
}

void SpriteManager::addspritesheet(sprites::name name, std::string pathtopng, int imagenumber, int imagetypes)
{
	spritemap[name] = std::make_unique<Spritesheet>(name, pathtopng, imagenumber, imagetypes);
}

Spritesheet* SpriteManager::get(sprites::name name)
{
	auto it = spritemap.find(name);
	if(it != spritemap.end())
		return it->second.get();
	else
		std::cout<<"Error: Failed to locate spritesheet with index "<<name<<std::endl;
}