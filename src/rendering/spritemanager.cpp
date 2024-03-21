#include<iostream>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"


SpriteManager::SpriteManager()
{
	spritemap[sprites::openwagon] = std::make_unique<Sprite>(sprites::openwagon, "rollingstock/openwagon.png");
	spritemap[sprites::refrigeratorcar] = std::make_unique<Sprite>(sprites::refrigeratorcar, "rollingstock/refrigeratorcar.png");
	spritemap[sprites::tankloco] = std::make_unique<Sprite>(sprites::tankloco, "rollingstock/loco0.png", 4);
	spritemap[sprites::iconopenwagon] = std::make_unique<Sprite>(sprites::iconopenwagon, "icons/openwagon.png");
	spritemap[sprites::iconrefrigeratorcar] = std::make_unique<Sprite>(sprites::iconrefrigeratorcar, "icons/refrigeratorcar.png");
	spritemap[sprites::icontankloco] = std::make_unique<Sprite>(sprites::icontankloco, "icons/loco.png");
}

Sprite* SpriteManager::get(sprites::name sprite)
{
	auto it = spritemap.find(sprite);
	if(it != spritemap.end())
		return it->second.get();
	else
		std::cout<<"Error: Failed to locate sprite with index "<<sprite<<std::endl;
}