#include<iostream>
#include "bahnhof/graphics/sprite.h"

void Sprite::setspritesheet(SpriteManager& s, sprites::name name)
{
	spritesheet = s.get(name);
	imagenumber = spritesheet->getimagenumber();
	if(imagenumber<1) std::cout<<"Error: sprite's image number is nonpositive"<<std::endl; 
	imagetypes = spritesheet->getimagetypes();
	if(imagetypes<1) std::cout<<"Error: sprite's number of types is nonpositive"<<std::endl; 
}

void Sprite::updateframe(int ms)
{
	imageindex += imagespeed*ms*0.001;
	if(imageindex>=imagenumber)
		imageindex -= imagenumber;
	if(imageindex<0)
		imageindex += imagenumber;
}

void Sprite::render(Rendering* r, Vec pos)
{
	if(spritesheet)
		spritesheet->render(r, pos, ported, zoomed, imageangle, imageindex, imagetype);
	else
		std::cout<<"Error: sprite tried rendering nonexisting spritesheet, did you forget to call setsprite?"<<std::endl;
}