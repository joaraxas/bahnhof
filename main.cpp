#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include "utils.h"


int main(){
	init();
	Tracksystem tracksystem({200,300,200}, {200,300,400});
	Train train(&tracksystem);
	bool quit = false;
	int ms = 0;
	int startTime = SDL_GetTicks();
	int lastTime = SDL_GetTicks();
	SDL_Texture* fieldtex = loadImage("assets/field.png");
	SDL_Event e;
	while(!quit){
		while(SDL_PollEvent(&e)){
			if(e.type==SDL_QUIT) quit = true;
		}
		keys = SDL_GetKeyboardState(NULL);

		ms = SDL_GetTicks() - lastTime;
		lastTime = SDL_GetTicks();
		train.update(ms);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		for(int x=0; x<SCREEN_WIDTH; x+=128){
		for(int y=0; y<SCREEN_HEIGHT; y+=128){
			SDL_Rect rect = {x,y,128,128};
			SDL_RenderCopy(renderer, fieldtex, NULL, &rect);
		}}
		tracksystem.render();
		train.render();
		int xMouse, yMouse;
		SDL_GetMouseState(&xMouse, &yMouse);
		SDL_RenderPresent(renderer);

	}
	close();
}
