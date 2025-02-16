#include<iostream>
#include "bahnhof/graphics/sprite.h"
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
	origin = Vec(int(w/2), int(h/2));
}

Spritesheet::~Spritesheet()
{
    SDL_DestroyTexture(tex);
}

void Spritesheet::render(Rendering* r, Vec pos, bool ported, bool zoomed, float imageangle, int imageindex, int imagetype, float imagescale)
{
	int x = int(pos.x);
	int y = int(pos.y);
	srcrect = {int(imagetype)*w, int(imageindex)*h, w, h};
	rect = {int(x - origin.x*imagescale), int(y - origin.y*imagescale), int(w*imagescale), int(h*imagescale)};
	if(origin.x==int(w/2) && origin.y==int(h/2))
		r->rendertexture(tex, &rect, &srcrect, imageangle, ported, zoomed);
	else
		r->rendertexture(tex, &rect, &srcrect, imageangle, ported, zoomed, false, origin.x, origin.y);
}

int Spritesheet::getimagenumber()
{
	return imagenumber;
}

int Spritesheet::getimagetypes()
{
	return imagetypes;
}

void Spritesheet::setoriginx(int neworiginx)
{
	origin.x = neworiginx;
}

void Spritesheet::setoriginy(int neworiginy)
{
	origin.y = neworiginy;
}

Vec Spritesheet::getsize()
{
	return Vec(w, h);
}

sprites::name Spritesheet::getname()
{
	if(name == sprites::name::none)
		std::cout<<"Warning: spritesheet with name none was called";
	return name;
}