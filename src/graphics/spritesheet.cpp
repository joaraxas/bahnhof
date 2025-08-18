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
	origin_x = int(w/2);
	origin_y = int(h/2);
}

Spritesheet::~Spritesheet()
{
    SDL_DestroyTexture(tex);
}

void Spritesheet::render(Rendering* r, Vec pos, bool ported, bool zoomed, float imageangle, int imageindex, 
						int imagetype, float imagescale, Uint8 imagealpha, Uint8 red, Uint8 green, Uint8 blue)
{
	int x = int(pos.x);
	int y = int(pos.y);
	srcrect = {int(imagetype)*w, int(imageindex)*h, w, h};
	rect = {int(x - origin_x*imagescale), int(y - origin_y*imagescale), int(w*imagescale), int(h*imagescale)};
	if(imagealpha!=alpha){
		SDL_SetTextureAlphaMod(tex, imagealpha);
		alpha = imagealpha;
	}
	if(red!=rgb[0] || green!=rgb[1] || blue!=rgb[2]){
		SDL_SetTextureColorMod(tex, red, green, blue);
		rgb[0] = red;
		rgb[1] = green;
		rgb[2] = blue;
	}
	if(origin_x==int(w/2) && origin_y==int(h/2))
		r->rendertexture(tex, &rect, &srcrect, imageangle, ported, zoomed, true);
	else
		r->rendertexture(tex, &rect, &srcrect, imageangle, ported, zoomed, false, origin_x*imagescale, origin_y*imagescale);
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
	origin_x = neworiginx;
}

void Spritesheet::setoriginy(int neworiginy)
{
	origin_y = neworiginy;
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