#include<iostream>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"

Sprite::Sprite(sprites::name newname, std::string pathtopng, int nimages, int ntypes) : name(newname)
{
	tex = loadimage(pathtopng);
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    imagenumber = nimages;
	h = h/imagenumber;
    imagetypes = ntypes;
    w = w/ntypes;
}

Sprite::~Sprite()
{
    SDL_DestroyTexture(tex);
}

void Sprite::render(Rendering* r, Vec pos, bool ported, bool zoomed, float imageangle, int imageindex, int imagetype)
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

int Sprite::getimagenumber()
{
	return imagenumber;
}

int Sprite::getimagetypes()
{
	return imagetypes;
}

void Animation::setsprite(SpriteManager* s, sprites::name name)
{
	sprite = s->get(name);
	imagenumber = sprite->getimagenumber();
	if(imagenumber<1) std::cout<<"Error: sprite's image number is nonpositive"<<std::endl; 
	imagetypes = sprite->getimagetypes();
	if(imagetypes<1) std::cout<<"Error: sprite's number of types is nonpositive"<<std::endl; 
}

void Animation::updateframe(int ms)
{
	imageindex += imagespeed*ms*0.001;
	if(imageindex>=imagenumber)
		imageindex -= imagenumber;
	if(imageindex<0)
		imageindex += imagenumber;
	std::cout<<imageindex<<std::endl;
}

void Animation::render(Rendering* r, Vec pos)
{
	if(sprite)
		sprite->render(r, pos, ported, zoomed, imageangle, imageindex, imagetype);
	else
		std::cout<<"Error: animation tried rendering nonexisting sprite, did you forget to call setsprite?"<<std::endl;
}