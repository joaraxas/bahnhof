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
	using namespace sprites;
	addspritesheet(openwagon, "rollingstock/openwagon.png");
	addspritesheet(refrigeratorcar, "rollingstock/refrigeratorcar.png");
	addspritesheet(tankloco, "rollingstock/loco0.png", 4);

	addspritesheet(iconopenwagon, "icons/openwagon.png");
	addspritesheet(iconrefrigeratorcar, "icons/refrigeratorcar.png");
	addspritesheet(icontankloco, "icons/loco.png");

	addspritesheet(switchsprite, "track/switch.png", 1, 2);
	addspritesheet(sprites::signal, "track/signal.png", 1, 2);
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