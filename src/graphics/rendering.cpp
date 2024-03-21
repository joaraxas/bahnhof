#pragma once
#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/track/track.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/rollingstock.h"
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
	gamestate->tracksystem->render(this);
	for(auto& wagon : gamestate->wagons)
		wagon->render(this);
	if(gamestate->routing->selectedroute)
		gamestate->routing->selectedroute->render(this);
	else
		gamestate->routing->renderroutes(this);
	for(auto& train : gamestate->trains)
		train->render(this);
	gamestate->tracksystem->renderabovetrains(this);
	rendertext(std::to_string(int(gamestate->money)) + " Fr", 20, 2*14, {static_cast<Uint8>(127*(gamestate->money<0)),static_cast<Uint8>(63*(gamestate->money>=0)),0,0}, false, false);
	rendertext(std::to_string(int(gamestate->time*0.001/60)) + " min", 20, 3*14, {0,0,0,0}, false, false);
	rendertext(std::to_string(int(60*float(gamestate->revenue)/float(gamestate->time*0.001/60))) + " Fr/h", 20, 4*14, {0,0,0,0}, false, false);
	SDL_SetRenderDrawColor(renderer, 0,0,0,255);
	int scalelinelength = 200;
	renderline(Vec(20,SCREEN_HEIGHT-20), Vec(20+scalelinelength,SCREEN_HEIGHT-20), false);
	renderline(Vec(20,SCREEN_HEIGHT-20-2), Vec(20,SCREEN_HEIGHT-20+2), false);
	renderline(Vec(20+scalelinelength,SCREEN_HEIGHT-20-2), Vec(20+scalelinelength,SCREEN_HEIGHT-20+2), false);
	rendertext(std::to_string(int(scalelinelength*0.001*150/getscale())) + " m", 20+scalelinelength*0.5-20, SCREEN_HEIGHT-20-14, {0,0,0,0}, false, false);
	SDL_SetRenderDrawColor(renderer, 255,255,255,255);
	SDL_RenderPresent(renderer);
}

void Rendering::rendertext(std::string text, int x, int y, SDL_Color color, bool ported, bool zoomed)
{
	SDL_Texture* tex = loadtext(text, color);
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	SDL_Rect rect = {x, y, w, h};
	rendertexture(tex, &rect, nullptr, 0, ported, zoomed);
	SDL_DestroyTexture(tex);
}

void Rendering::rendertexture(SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect, float angle, bool ported, bool zoomed)
{
	if(ported){
		Vec screenpos = cam->screencoord(Vec(rect->x, rect->y));
		rect->x = screenpos.x; rect->y = screenpos.y;
	}
	if(zoomed){
		rect->w *= cam->getscale();
		rect->h *= cam->getscale();
	}
	SDL_RenderCopyEx(renderer, tex, srcrect, rect, -angle * 180 / pi, NULL, SDL_FLIP_NONE);
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

void Rendering::renderrectangle(SDL_Rect* rect, bool ported, bool zoomed)
{
	if(ported){
		Vec screenpos = cam->screencoord(Vec(rect->x, rect->y));
		rect->x = screenpos.x; rect->y = screenpos.y;
	}
	if(zoomed){
		rect->w *= cam->getscale();
		rect->h *= cam->getscale();
	}
	SDL_RenderDrawRect(renderer, rect);
}

void Rendering::renderfilledrectangle(SDL_Rect* rect, bool ported, bool zoomed)
{
	if(ported){
		Vec screenpos = cam->screencoord(Vec(rect->x, rect->y));
		rect->x = screenpos.x; rect->y = screenpos.y;
	}
	if(zoomed){
		rect->w *= cam->getscale();
		rect->h *= cam->getscale();
	}
	SDL_RenderFillRect(renderer, rect);
}

float Rendering::getscale()
{
	return cam->getscale();
}