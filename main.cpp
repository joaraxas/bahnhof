#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<string>
#include<map>
#include "utils.h"

std::vector<std::unique_ptr<Wagon>> wagons;
float money;
bool nicetracks = false;
float scale = 1;
int xMouse, yMouse;

int main(){
	init();
	ResourceManager resources;
	money = 0;
	//Tracksystem tracksystem(resources, {200,300,700,800,800,800,700,300,200,200,300,600,650,600,100}, {200,200,200,300,500,600,700,700,600,500,400,400,350,300,300});
	//Tracksystem tracksystem(resources, {200,300,700}, {200,200,200});
	Tracksystem tracksystem(resources, {200,700,800,800,700,200,100,100}, {200,200,300,600,700,700,600,300});
	tracksystem.leftclick(200, 200);
	tracksystem.rightclick(0, 0);
	signalid beforefields = tracksystem.addsignal(State(4,0.9,true));
	//tracksystem.addsignal(State(7,0.5,true));
	wagons.emplace_back(new Locomotive(tracksystem, State(2,0.5,true)));
	wagons.emplace_back(new Locomotive(tracksystem, State(4,0.5,true)));
	for(int iWagon=0; iWagon<6; iWagon++){
		State state = tracksystem.travel(State(1, 0.2, true), iWagon*60);
		wagons.emplace_back(new Openwagon(tracksystem, state));
	}
	for(int iWagon=0; iWagon<1; iWagon++){
		State state = tracksystem.travel(State(3, 0.5, true), iWagon*80);
		wagons.emplace_back(new Tankwagon(tracksystem, state));
	}
	
	for(int iWagon=0; iWagon<wagons.size(); iWagon++){
		trains.emplace_back(new Train(tracksystem, {wagons[iWagon].get()}, 0));
	}
	trains[0]->selected = true;
	storages.emplace_back(new Storage(resources, 100,100,400,150, hops, beer));
	storages.emplace_back(new Storage(resources, 300,600,600,150, beer, hops));
	Brewery brewery(resources, 150,120,100,50);
	Hopsfield farm(resources, 625,625,50,50);
	City city(resources, 700,625,20,50);

	Route route1;
	route1.appendorder(new Turn());
	route1.appendorder(new Gotostate(State(1,0.1,true)));
	route1.appendorder(new Turn());
	route1.appendorder(new Wipe());
	route1.appendorder(new Loadresource());
	route1.appendorder(new Gotostate(State(5,0.1,true)));
	route1.appendorder(new Setsignal(beforefields, 0));
	route1.appendorder(new Gotostate(State(5,0.7,true)));
	route1.appendorder(new Loadresource());
	route1.appendorder(new Gotostate(State(7,0.9,true)));
	route1.appendorder(new Setsignal(beforefields, 1));
	//route1.appendorder(new Decouple());
	//route1.appendorder(new Gotostate(State(5,0.9,true)));
	//route1.appendorder(new Turn());
	//route1.appendorder(new Gotostate(State(5,0.4,true)));
	//route1.appendorder(new Turn());
	route1.appendorder(new Gotostate(State(1,0.5,true)));

	//route1.appendorder(new Decouple());
	//route1.appendorder(new Turn());
	trains[0]->route = &route1;

	Route route2;
	route2.appendorder(new Turn());
	route2.appendorder(new Gotostate(State(3,0.3,true)));
	route2.appendorder(new Turn());
	route2.appendorder(new Wipe());
	route2.appendorder(new Gotostate(State(5,0.1,true)));
	route2.appendorder(new Setsignal(beforefields, 0));
	route2.appendorder(new Gotostate(State(5,0.7,true)));
	route2.appendorder(new Loadresource());
	route2.appendorder(new Gotostate(State(7,0.9,true)));
	route2.appendorder(new Setsignal(beforefields, 1));
	route2.appendorder(new Gotostate(State(1,0.5,true)));
	route2.appendorder(new Loadresource());
	trains[1]->route = &route2;

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
						tracksystem.deleteclick(xMouse, yMouse);
					}
					else if(e.button.button == SDL_BUTTON_LEFT){
						bool clickedtrain = false;
						for(auto& train : trains){
							train->selected = false;
							for(auto& wagon : train->wagons)
								if(norm(Vec(xMouse,yMouse)-wagon->pos)<wagon->w/2){
									train->selected = true;
									clickedtrain = true;
								}
						}
						if(!clickedtrain)
							tracksystem.leftclick(xMouse, yMouse);
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

		for(int iTrain=0; iTrain<trains.size(); iTrain++)
			trains[iTrain]->getinput(ms);
		for(int iTrain=0; iTrain<trains.size(); iTrain++)
			for(int jTrain=0; jTrain<trains.size(); jTrain++)
				if(iTrain!=jTrain)
					trains[iTrain]->checkCollision(ms, trains[jTrain].get());
		for(int iTrain=trains.size()-1; iTrain>=0; iTrain--)
			if(trains[iTrain]->wagons.size() == 0)
				trains.erase(trains.begin()+iTrain);
		for(int iTrain=0; iTrain<trains.size(); iTrain++){
			trains[iTrain]->update(ms);
		}
		for(auto& wagon : wagons)
			wagon->update(ms);
		brewery.update(ms);
		farm.update(ms);
		city.update(ms);

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
