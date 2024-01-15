#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include "utils.h"


int main(){
	init();
	Tracksystem tracksystem({200,300,600}, {200,210,400});
	Wagon locomotive(&tracksystem, 0.1);
	Wagon wagon(&tracksystem, 0.1+35/tracksystem.tracks[0]->getarclength(1));
	Wagon wagon2(&tracksystem, 0.1+70/tracksystem.tracks[0]->getarclength(1));
	trains.emplace_back(new Train(&tracksystem, {&locomotive,&wagon}, 0));
	trains.emplace_back(new Train(&tracksystem, {&wagon2}, 0));
	trains[0]->selected = true;
	trains[1]->selected = false;
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
						tracksystem.leftclick(xMouse, yMouse);
					}
					if(e.button.button == SDL_BUTTON_MIDDLE){
						tracksystem.nodes.pop_back();
						tracksystem.tracks.pop_back();
						tracksystem.selectednode = nullptr;
					}
					if(e.button.button == SDL_BUTTON_RIGHT){
						tracksystem.rightclick(xMouse, yMouse);
					}
					break;
			}
		}
		keys = SDL_GetKeyboardState(NULL);
		for(int iTrain=trains.size()-1; iTrain>=0; iTrain--){
			trains[iTrain]->getinput();
		}
		for(int iTrain=trains.size()-1; iTrain>=0; iTrain--)
			if(trains[iTrain]->wagons.size() == 0)
				trains.erase(trains.begin()+iTrain);

		ms = SDL_GetTicks() - lastTime;
		lastTime = SDL_GetTicks();
		locomotive.update(ms);
		wagon.update(ms);
		wagon2.update(ms);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		for(int x=0; x<SCREEN_WIDTH; x+=128){
		for(int y=0; y<SCREEN_HEIGHT; y+=128){
			SDL_Rect rect = {x,y,128,128};
			SDL_RenderCopy(renderer, fieldtex, NULL, &rect);
		}}
		tracksystem.render();
		locomotive.render();
		wagon.render();
		wagon2.render();
		SDL_GetMouseState(&xMouse, &yMouse);
		SDL_RenderPresent(renderer);
	}
	close();
}
