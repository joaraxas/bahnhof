#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include "utils.h"

int main(){
	init();

	Resource beer(BEER, "Beer", "assets/beer.png");
	Resource hops(HOPS, "Hops", "assets/beer.png");
	selectedresource = &beer;
	Tracksystem tracksystem({200,300,800}, {200,210,400});
	for(int iWagon=0; iWagon<11; iWagon++){
		wagons.emplace_back(new Wagon(&tracksystem, 0.5+iWagon*50/tracksystem.tracks[0]->getarclength(1)));
	}
	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		trains.emplace_back(new Train(&tracksystem, {wagons[iWagon].get()}, 0));
	}
	trains[0]->selected = true;
	wagons[0]->alignedwithtrackdirection = false;
	wagons[0]->P[1] = 0.2;
	wagons[0]->P[0] = 0.2;
	wagons[0]->maxamount = 0;
	wagons[1]->loadwagon(beer, 12);
	wagons[3]->loadwagon(hops, 12);
	
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
		ms = SDL_GetTicks() - lastTime;
		lastTime = SDL_GetTicks();

		for(int iTrain=trains.size()-1; iTrain>=0; iTrain--){
			trains[iTrain]->getinput(ms);
		}
		for(auto& wagon : wagons)
			wagon->update(ms);
		for(int iTrain=0; iTrain<trains.size(); iTrain++){
			for(int jTrain=iTrain+1; jTrain<trains.size(); jTrain++){
				trains[iTrain]->checkCollision(trains[jTrain].get());
			}
		}
		for(int iTrain=trains.size()-1; iTrain>=0; iTrain--)
			if(trains[iTrain]->wagons.size() == 0)
				trains.erase(trains.begin()+iTrain);


		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		for(int x=0; x<SCREEN_WIDTH; x+=128){
		for(int y=0; y<SCREEN_HEIGHT; y+=128){
			SDL_Rect rect = {x,y,128,128};
			SDL_RenderCopy(renderer, fieldtex, NULL, &rect);
		}}
		tracksystem.render();
		for(auto& wagon : wagons)
			wagon->render();
		SDL_GetMouseState(&xMouse, &yMouse);
		SDL_RenderPresent(renderer);
	}

	close();
}
