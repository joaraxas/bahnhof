#include<iostream>
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"


SpriteManager::SpriteManager(Game* g) : game(g)
{
	float hdscale = game->getrendering().getlogicalscale();
	using namespace sprites;
	addspritesheet(openwagon, "rollingstock/openwagon.png");
	addspritesheet(refrigeratorcar, "rollingstock/refrigeratorcar.png");
	addspritesheet(tankloco, "rollingstock/loco0.png", 4);

	addspritesheet(iconopenwagon, "icons/openwagon.png", 1, 1, hdscale);
	addspritesheet(iconrefrigeratorcar, "icons/refrigeratorcar.png", 1, 1, hdscale);
	addspritesheet(icontankloco, "icons/loco.png", 1, 1, hdscale);

	addspritesheet(switchsprite, "track/switch.png", 1, 2, hdscale);
	get(switchsprite)->setoriginy(15);
	addspritesheet(sprites::signal, "track/signal.png", 1, 2, hdscale);
	get(sprites::signal)->setoriginx(3);
	get(sprites::signal)->setoriginy(31);

	addspritesheet(light, "effects/light.png", 1, 2);

	addspritesheet(beer, "resources/beer.png", 1, 1, hdscale);
	addspritesheet(hops, "resources/hops.png", 1, 1, hdscale);
	addspritesheet(barley, "resources/barley.png", 1, 1, hdscale);
}

void SpriteManager::addspritesheet(sprites::name name, std::string pathtopng, int imagenumber, int imagetypes, float imagescale)
{
	spritemap[name] = std::make_unique<Spritesheet>(name, pathtopng, imagenumber, imagetypes, imagescale);
}

Spritesheet* SpriteManager::get(sprites::name name)
{
	auto it = spritemap.find(name);
	if(it != spritemap.end())
		return it->second.get();
	else
		std::cout<<"Error: Failed to locate spritesheet with index "<<name<<std::endl;
}