#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include "utils.h"


int main(){
	init();
	Tracksystem tracksystem({200,300}, {200,150});
	//Train train(&tracksystem);
	bool quit = false;
	int ms = 0;
	int startTime = SDL_GetTicks();
	int lastTime = SDL_GetTicks();
	int xMouse, yMouse;
	SDL_Texture* fieldtex = loadImage("assets/field.png");
	SDL_Event e;
	while(!quit){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT: 
					quit = true; break;
				case SDL_MOUSEBUTTONDOWN:
					SDL_GetMouseState(&xMouse, &yMouse);
					if(e.button.button == SDL_BUTTON_LEFT){
						tracksystem.addnode(xMouse, yMouse, tracksystem.nodes.back().get());
						tracksystem.addtrack(tracksystem.nodes[tracksystem.nodes.size() - 2].get(), tracksystem.nodes.back().get(), tracksystem.nodes.size()-1);
					}
					if(e.button.button == SDL_BUTTON_MIDDLE){
						tracksystem.nodes.pop_back();
						tracksystem.tracks.pop_back();
					}
					break;
			}
		}
		keys = SDL_GetKeyboardState(NULL);

		ms = SDL_GetTicks() - lastTime;
		lastTime = SDL_GetTicks();
		//train.update(ms);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		for(int x=0; x<SCREEN_WIDTH; x+=128){
		for(int y=0; y<SCREEN_HEIGHT; y+=128){
			SDL_Rect rect = {x,y,128,128};
			SDL_RenderCopy(renderer, fieldtex, NULL, &rect);
		}}
		tracksystem.render();
		//train.render();
		SDL_GetMouseState(&xMouse, &yMouse);
		tracksystem.nodes.back()->pos.x = xMouse;
		tracksystem.nodes.back()->pos.y = yMouse;
		int nNodes = tracksystem.nodes.size();
		float dx = xMouse - tracksystem.nodes[nNodes-2]->pos.x;
		float dy = -(yMouse - tracksystem.nodes[nNodes-2]->pos.y);
		tracksystem.nodes.back()->dir = 2*atan2(dy,dx) - tracksystem.nodes[nNodes-2]->dir;
		if(tracksystem.nodes.back()->dir-tracksystem.nodes[nNodes-2]->dir < -2*pi)
			tracksystem.nodes.back()->dir += 4*pi;
		if(tracksystem.nodes.back()->dir-tracksystem.nodes[nNodes-2]->dir > 2*pi)
			tracksystem.nodes.back()->dir -= 4*pi;
		tracksystem.tracks.back()->radius=tracksystem.tracks.back()->getradius();
		SDL_RenderPresent(renderer);

	}
	close();
}
