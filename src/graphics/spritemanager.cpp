#include<iostream>
#include "bahnhof/graphics/sprite.h"


SpriteManager::SpriteManager()
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

	addspritesheet(light, "effects/light.png", 1, 2);

	addspritesheet(beer, "resources/beer.png");
	addspritesheet(hops, "resources/hops.png");
	addspritesheet(barley, "resources/barley.png");
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