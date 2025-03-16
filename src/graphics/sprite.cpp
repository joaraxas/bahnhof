#include<iostream>
#include "bahnhof/graphics/sprite.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/ui/ui.h"

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
		spritesheet->render(r, pos, ported, zoomed, imageangle, imageindex, imagetype, imagescale);
	else
		std::cout<<"Error: sprite tried rendering nonexisting spritesheet, did you forget to call setsprite?"<<std::endl;
}

Vec Sprite::getsize()
{
	Vec sheetsize = spritesheet->getsize()*imagescale;
	return Vec(sheetsize.x/imagetypes, sheetsize.y/imagenumber);
}

sprites::name Sprite::getname()
{	
	sprites::name sheetname = spritesheet->getname();
	if(sheetname == sprites::name::none)
		std::cout<<"Warning: sprite with spritesheet with name none was called";
	return sheetname;
}

Icon::Icon()
{
	zoomed = false;
}

void Icon::setspritesheet(SpriteManager& s, sprites::name name)
{
	imagescale = s.getgame()->getui().getuirendering().getuiscale();
	Sprite::setspritesheet(s, name);
}

void Icon::render(Rendering* r, Vec pos)
{
	imagescale = r->getgame().getui().getuirendering().getuiscale();
	Sprite::render(r, pos);
}