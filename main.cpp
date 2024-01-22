#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

std::vector<std::unique_ptr<Wagon>> wagons;
float money;
bool nicetracks = true;
float scale = 1;

int main(){
	init();

	Resource beer(BEER, "Beer", "assets/beer.png");
	Resource hops(HOPS, "Hops", "assets/hops.png");
	selectedresource = &beer;
	money = 0;
	Tracksystem tracksystem({200,300,700,800,800,800,700,300,200,200,300,600,650,600,100}, {200,200,200,300,500,600,700,700,600,500,400,400,350,300,300});
	wagons.emplace_back(new Wagon(&tracksystem, 0.5, "assets/loco0.png"));
	for(int iWagon=1; iWagon<11; iWagon++){
		wagons.emplace_back(new Wagon(&tracksystem, 0.5+iWagon*80/scale/tracksystem.tracks[0]->getarclength(1), "assets/train.png"));
	}
	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		trains.emplace_back(new Train(&tracksystem, {wagons[iWagon].get()}, 0));
	}
	trains[0]->selected = true;
	wagons[0]->alignedwithtrackdirection = false;
	wagons[0]->P[1] = 0.2*4;
	wagons[0]->P[0] = 0.2*4;
	wagons[0]->maxamount = 0;
	wagons[1]->loadwagon(beer, 1);
	wagons[3]->loadwagon(hops, 1);
	storages.emplace_back(new Storage(100,100,400,150));
	storages.emplace_back(new Storage(600,600,300,100));
	storages[0]->loadstorage(beer, 4);
	Building brewery(150,120,100,50, &hops, &beer);
	Building farm(625,625,50,50, nullptr, &hops);
	Building city(700,625,20,50, &beer, nullptr);
	
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
		brewery.update(ms);
		farm.update(ms);
		city.update(ms);
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
		brewery.render();
		farm.render();
		city.render();
		for(auto& storage : storages)
			storage->render();
		tracksystem.render();
		for(auto& wagon : wagons)
			wagon->render();
		SDL_GetMouseState(&xMouse, &yMouse);
		SDL_RenderPresent(renderer);
	}

	close();
}
