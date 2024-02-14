#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<map>
#include "utils.h"

float money = 0;
bool nicetracks = true;
float scale = 1;
int xMouse, yMouse;

int main(){
	init();
	Gamestate gamestate;
	bool quit = false;
	int ms = 0;
	int startTime = SDL_GetTicks();
	int lastTime = SDL_GetTicks();
	SDL_Texture* fieldtex = loadImage("assets/field.png");
	SDL_Event e;

	while(!quit){
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT: 
					quit = true; break;
				case SDL_MOUSEBUTTONDOWN:
					SDL_GetMouseState(&xMouse, &yMouse);
					if(e.button.button == SDL_BUTTON_LEFT && keys[gearbutton]){
						gamestate.tracksystem->deleteclick(xMouse, yMouse);
					}
					else if(e.button.button == SDL_BUTTON_LEFT){
						if(gamestate.tracksystem->selectednode)
							gamestate.tracksystem->buildat(Vec(xMouse, yMouse));
						else{
							bool clickedtrain = false;
							for(auto& train : trains){
								train->selected = false;
								for(auto& wagon : train->wagons){
									if(norm(Vec(xMouse,yMouse)-wagon->pos)<wagon->w/2){
										train->selected = true;
										gamestate.selectedroute = train->route;
										clickedtrain = true;
									}
									if(clickedtrain) break;
								}
							}
							if(!clickedtrain)
								gamestate.tracksystem->selectat(Vec(xMouse, yMouse));
						}
					}
					if(e.button.button == SDL_BUTTON_RIGHT){
						gamestate.tracksystem->selectednode = 0;
						gamestate.tracksystem->switchat(Vec(xMouse, yMouse));
					}
					break;
			}
		}
		keys = SDL_GetKeyboardState(NULL);
		ms = SDL_GetTicks() - lastTime;
		lastTime = SDL_GetTicks();

		for(int iTrain=0; iTrain<trains.size(); iTrain++)
			trains[iTrain]->getinput(ms);
		for(int iTrain=0; iTrain<trains.size(); iTrain++)
			for(int jTrain=0; jTrain<trains.size(); jTrain++)
				if(iTrain!=jTrain)
					trains[iTrain]->checkcollision(ms, trains[jTrain].get());
		for(int iTrain=trains.size()-1; iTrain>=0; iTrain--)
			if(trains[iTrain]->wagons.size() == 0)
				trains.erase(trains.begin()+iTrain);
		for(int iTrain=0; iTrain<trains.size(); iTrain++){
			trains[iTrain]->update(ms);
		}
		for(auto& wagon : gamestate.wagons)
			wagon->update(ms);
		for(auto& building : buildings)
			building->update(ms);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		for(int x=0; x<SCREEN_WIDTH; x+=128){
		for(int y=0; y<SCREEN_HEIGHT; y+=128){
			SDL_Rect rect = {x,y,128,128};
			SDL_RenderCopy(renderer, fieldtex, NULL, &rect);
		}}
		for(auto& building : buildings)
			building->render();
		for(auto& storage : storages)
			storage->render();
		gamestate.tracksystem->render();
		for(auto& wagon : gamestate.wagons)
			wagon->render();
		if(gamestate.selectedroute)
			gamestate.selectedroute->render();
		for(auto& train : trains)
			train->render();
		SDL_GetMouseState(&xMouse, &yMouse);
		SDL_RenderPresent(renderer);
	}

	close();
}
