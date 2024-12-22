#pragma once
#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/input.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/track/track.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/resources/storage.h"

Vec randpos(int xoffset, int yoffset)
{
	return Vec(randint(MAP_WIDTH-xoffset), randint(MAP_HEIGHT-yoffset));
};


Rendering::Rendering(Game* whatgame, Camera* whatcam)
{
	game = whatgame;
	cam = whatcam;
	fieldtex = loadimage("backgrounds/field.png");
}

void Rendering::render(Gamestate* gamestate)
{
	Tracks::Tracksystem& tracksystem = gamestate->gettracksystems();
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	for(int x=0; x<MAP_WIDTH; x+=128){
	for(int y=0; y<MAP_HEIGHT; y+=128){
		SDL_Rect rect = {x,y,128,128};
		rendertexture(fieldtex, &rect);
	}}
	for(auto& building : gamestate->buildings)
		building->render(this);
	for(auto& storage : storages)
		storage->render(this);
	Tracks::render(tracksystem, this);
	game->getinputmanager().render(this, tracksystem);
	gamestate->gettrains().render(this);
	Tracks::renderabovetrains(tracksystem, this);

	InterfaceManager& ui = game->getui();
	ui.render(this);

	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderPresent(renderer);
}

SDL_Rect Rendering::rendertext(std::string text, int x, int y, SDL_Color color, bool ported, bool zoomed, int maxwidth)
{
	SDL_Texture* tex = loadtext(text, color, maxwidth);
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	SDL_Rect rect = {x, y, w, h};
	rendertexture(tex, &rect, nullptr, 0, ported, zoomed);
	SDL_DestroyTexture(tex);
	return rect;
}

int Rendering::rendertable(std::vector<std::string> lines, SDL_Rect maxarea){
    int scale = getlogicalscale();
    int maxwidth = maxarea.w;
    int linedist = 3;
    int yoffset = 0;
    for(auto& line: lines){
        SDL_Rect textrect = rendertext(line, maxarea.x, maxarea.y+yoffset, {0,0,0,0}, false, false, maxwidth);
        yoffset += textrect.h + linedist;
    }
    return yoffset;
}

void Rendering::rendertexture(SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect, float angle, bool ported, bool zoomed, bool originiscenter, int centerx, int centery)
{
	if(ported){
		Vec screenpos = cam->screencoord(Vec(rect->x, rect->y));
		rect->x = screenpos.x; rect->y = screenpos.y;
	}
	if(zoomed){
		rect->w *= cam->getscale();
		rect->h *= cam->getscale();
	}
	if(originiscenter)
		SDL_RenderCopyEx(renderer, tex, srcrect, rect, -angle * 180 / pi, NULL, SDL_FLIP_NONE);
	else{
		SDL_Point center = {centerx, centery};
		SDL_RenderCopyEx(renderer, tex, srcrect, rect, -angle * 180 / pi, &center, SDL_FLIP_NONE);
	}
}

void Rendering::renderline(Vec pos1, Vec pos2, bool ported)
{
	if(ported){
		pos1 = cam->screencoord(pos1);
		pos2 = cam->screencoord(pos2);
	}
	//if((pos1.x>0 && pos1.x<SCREEN_WIDTH && pos1.y>0 && pos1.y<SCREEN_HEIGHT) || 
	//	(pos2.x>0 && pos2.x<SCREEN_WIDTH && pos2.y>0 && pos2.y<SCREEN_HEIGHT))
		SDL_RenderDrawLine(renderer, pos1.x, pos1.y, pos2.x, pos2.y);
}

void Rendering::renderrectangle(SDL_Rect rect, bool ported, bool zoomed)
{
	if(ported){
		Vec screenpos = cam->screencoord(Vec(rect.x, rect.y));
		rect.x = screenpos.x; rect.y = screenpos.y;
	}
	if(zoomed){
		rect.w *= cam->getscale();
		rect.h *= cam->getscale();
	}
	SDL_RenderDrawRect(renderer, &rect);
}

void Rendering::renderfilledrectangle(SDL_Rect rect, bool ported, bool zoomed)
{
	if(ported){
		Vec screenpos = cam->screencoord(Vec(rect.x, rect.y));
		rect.x = screenpos.x; rect.y = screenpos.y;
	}
	if(zoomed){
		rect.w *= cam->getscale();
		rect.h *= cam->getscale();
	}
	SDL_RenderFillRect(renderer, &rect);
}

Vec Rendering::getviewsize()
{
	// Returns renderer size in logical pixels
	int windowwidthinpixels, windowheightinpixels;
    SDL_GetRendererOutputSize(renderer, &windowwidthinpixels, &windowheightinpixels);
	return Vec(windowwidthinpixels, windowheightinpixels);
}

int Rendering::getlogicalscale()
{
	int windowwidth, windowheight;
	SDL_GetWindowSize(window, &windowwidth, &windowheight);
	int windowwidthinpixels, windowheightinpixels;
	SDL_GetWindowSizeInPixels(window, &windowwidthinpixels, &windowheightinpixels);
	int xscale = windowwidthinpixels/windowwidth;
	int yscale = windowheightinpixels/windowheight;
	if(xscale!=yscale)
		std::cout<<"warning: Rendering::getlogicalsize() returns different yscale from xscale"<<std::endl;
	return xscale;
}

float Rendering::getcamscale()
{
	return cam->getscale();
}