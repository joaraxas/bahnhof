#include<iostream>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"

Spritesheet::Spritesheet(sprites::name newname, std::string pathtopng, int nimages, int ntypes) : name(newname)
{
	tex = loadimage(pathtopng);
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    imagenumber = nimages;
	h = h/imagenumber;
    imagetypes = ntypes;
    w = w/ntypes;
}

Spritesheet::~Spritesheet()
{
    SDL_DestroyTexture(tex);
}

void Spritesheet::render(Rendering* r, Vec pos, bool ported, bool zoomed, float imageangle, int imageindex, int imagetype)
{
	int x = int(pos.x);
	int y = int(pos.y);
	srcrect = {int(imagetype)*w, int(imageindex)*h, w, h};
    float scale = 1;
    if(!zoomed)
        scale = r->getscale();
	rect = {int(x - w/scale/2), int(y - h/scale/2), w, h};
	r->rendertexture(tex, &rect, &srcrect, imageangle, ported, zoomed);
}

int Spritesheet::getimagenumber()
{
	return imagenumber;
}

int Spritesheet::getimagetypes()
{
	return imagetypes;
}

SpriteManager::SpriteManager()
{
	spritemap[sprites::openwagon] = std::make_unique<Spritesheet>(sprites::openwagon, "rollingstock/openwagon.png");
	spritemap[sprites::refrigeratorcar] = std::make_unique<Spritesheet>(sprites::refrigeratorcar, "rollingstock/refrigeratorcar.png");
	spritemap[sprites::tankloco] = std::make_unique<Spritesheet>(sprites::tankloco, "rollingstock/loco0.png", 4);
	spritemap[sprites::iconopenwagon] = std::make_unique<Spritesheet>(sprites::iconopenwagon, "icons/openwagon.png");
	spritemap[sprites::iconrefrigeratorcar] = std::make_unique<Spritesheet>(sprites::iconrefrigeratorcar, "icons/refrigeratorcar.png");
	spritemap[sprites::icontankloco] = std::make_unique<Spritesheet>(sprites::icontankloco, "icons/loco.png");
}

Spritesheet* SpriteManager::get(sprites::name sprite)
{
	auto it = spritemap.find(sprite);
	if(it != spritemap.end())
		return it->second.get();
	else
		std::cout<<"Error: Failed to locate spritesheet with index "<<sprite<<std::endl;
}