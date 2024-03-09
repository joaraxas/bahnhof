#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<string>
#include<format>
#include<map>
#include "utils.h"

float money = 0;
bool nicetracks = true;
float scale = 1;
int xMouse, yMouse;
float gamespeed = 1;

int main(){
	init();
	Gamestate gamestate;
	bool quit = false;
	int ms = 0;
	int mslogic = ms*gamespeed;
	int startTime = SDL_GetTicks();
	int lastTime = SDL_GetTicks();
	SDL_Texture* fieldtex = loadImage("assets/field.png");
	SDL_Event e;

	while(!quit){
		keys = SDL_GetKeyboardState(nullptr);
		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_QUIT:{
					quit = true; break;}
				case SDL_MOUSEBUTTONDOWN:{
					SDL_GetMouseState(&xMouse, &yMouse);
					Vec mousepos(xMouse/scale+cam.x, yMouse/scale+cam.y);
					if(e.button.button == SDL_BUTTON_LEFT && keys[gearbutton]){
						gamestate.tracksystem->deleteclick(xMouse, yMouse);
					}
					else if(e.button.button == SDL_BUTTON_RIGHT){
						gamestate.tracksystem->selectednode = 0;
						gamestate.tracksystem->placingsignal = false;
						if(gamestate.selectedroute){
							Order* neworder = gamestate.tracksystem->generateorderat(mousepos);
							if(neworder)
								gamestate.selectedroute->insertorderatselected(neworder);
						}
					}
					if(e.button.button == SDL_BUTTON_LEFT){
						if(gamestate.tracksystem->selectednode || gamestate.tracksystem->placingsignal){
							if(money>0)
								money-=gamestate.tracksystem->buildat(mousepos);
						}
						else{
							Train* clickedtrain = nullptr;
							for(auto& train : trains){
								for(auto& wagon : train->wagons){
									if(norm(mousepos-wagon->pos)<wagon->w/2/scale){
										gamestate.selectedroute = train->route;
										clickedtrain = train.get();
									}
									if(clickedtrain) break;
								}
							}
							if(clickedtrain){
								for(auto& train : trains)
									train->selected = false;
								clickedtrain->selected = true;
							}
							else{
								if(!gamestate.tracksystem->switchat(mousepos)){
									gamestate.tracksystem->selectat(mousepos);
									for(auto& train : trains)
										train->selected = false;
									gamestate.selectedroute = nullptr;
								}
							}
						}
					}
					break;
					}
				case SDL_KEYDOWN:{
					if(e.key.keysym.sym == SDLK_r)
						if(!gamestate.selectedroute)
							gamestate.addroute();
					if(keys[routeassignbutton]){
						if(e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_0+gamestate.routes.size())
							for(auto& train : trains)
								if(train->selected){
									gamestate.selectedroute = gamestate.routes[e.key.keysym.sym-SDLK_0-1].get();
									train->route = gamestate.selectedroute;
								}
					}
					if(e.key.keysym.sym == SDLK_UP)
						if(gamestate.selectedroute)
							gamestate.selectedroute->selectedorderid = gamestate.selectedroute->previousorder(gamestate.selectedroute->selectedorderid);
					if(e.key.keysym.sym == SDLK_DOWN)
						if(gamestate.selectedroute)
							gamestate.selectedroute->selectedorderid = gamestate.selectedroute->nextorder(gamestate.selectedroute->selectedorderid);
					if(e.key.keysym.sym == SDLK_BACKSPACE)
						if(gamestate.selectedroute)
							gamestate.selectedroute->removeselectedorder();
					if(e.key.keysym.sym == SDLK_t)
						if(gamestate.selectedroute)
							gamestate.selectedroute->insertorderatselected(new Turn());
					if(e.key.keysym.sym == SDLK_e)
						if(gamestate.selectedroute)
							gamestate.selectedroute->insertorderatselected(new Decouple());
					if(e.key.keysym.sym == SDLK_l)
						if(gamestate.selectedroute)
							gamestate.selectedroute->insertorderatselected(new Loadresource());
					if(e.key.keysym.sym == SDLK_c)
						if(gamestate.selectedroute)
							gamestate.selectedroute->insertorderatselected(new Couple());
					if(e.key.keysym.sym == SDLK_p)
						for(auto& train : trains)
							if(train->selected)
								train->proceed();
					if(e.key.keysym.sym == SDLK_RETURN)
						for(auto& train : trains)
							if(train->selected){
								train->go = !train->go;
								train->speed = 0;
							}
					if(e.key.keysym.sym == SDLK_z)
						gamestate.tracksystem->placingsignal = true;
					if(e.key.keysym.sym == SDLK_n)
						nicetracks = !nicetracks;
					if(money>0){
						if(e.key.keysym.sym == SDLK_o){
							gamestate.wagons.emplace_back(new Openwagon(*gamestate.tracksystem, gamestate.newwagonstate));
							gamestate.newwagonstate = gamestate.tracksystem->travel(gamestate.newwagonstate, 60);
							gamestate.addtrainstoorphans();
							money -= 3;
						}
						if(e.key.keysym.sym == SDLK_q){
							gamestate.wagons.emplace_back(new Tankwagon(*gamestate.tracksystem, gamestate.newwagonstate));
							gamestate.newwagonstate = gamestate.tracksystem->travel(gamestate.newwagonstate, 72);
							gamestate.addtrainstoorphans();
							money -= 3;
						}
						if(e.key.keysym.sym == SDLK_y){
							gamestate.wagons.emplace_back(new Locomotive(*gamestate.tracksystem, gamestate.newwagonstate));
							gamestate.newwagonstate = gamestate.tracksystem->travel(gamestate.newwagonstate, 60);
							gamestate.addtrainstoorphans();
							money -= 8;
						}
					}
					break;
				}
				case SDL_MOUSEWHEEL:{
					SDL_GetMouseState(&xMouse, &yMouse);
					if(e.wheel.y > 0){ // zoom in
						cam.x+=cam.w/2*xMouse/SCREEN_WIDTH;
						cam.w/=2;
						cam.y+=cam.h/2*yMouse/SCREEN_HEIGHT;
						cam.h/=2;
						scale*=2;
						cam.x = fmax(0, fmin(MAP_WIDTH-cam.w, cam.x));
						cam.y = fmax(0, fmin(MAP_HEIGHT-cam.h, cam.y));
						gamespeed=gamespeed/1.5;
					}
					if(e.wheel.y < 0){ // zoom out
						cam.x-=cam.w*xMouse/SCREEN_WIDTH;
						cam.w*=2;
						cam.y-=cam.h*yMouse/SCREEN_HEIGHT;
						cam.h*=2;
						scale/=2;
						cam.x = fmax(0, fmin(MAP_WIDTH-cam.w, cam.x));
						cam.y = fmax(0, fmin(MAP_HEIGHT-cam.h, cam.y));
						gamespeed=gamespeed*1.5;
					}
					break;
				}
			}
		}
		if(keys[leftpanbutton])
			cam.x = fmax(0, cam.x-fmax(1,int(ms*0.4/scale)));
		if(keys[rightpanbutton])
			cam.x = fmin(MAP_WIDTH-cam.w, cam.x+fmax(1,int(ms*0.4/scale)));
		if(keys[uppanbutton])
			cam.y = fmax(0, cam.y-fmax(1,int(ms*0.4/scale)));
		if(keys[downpanbutton])
			cam.y = fmin(MAP_HEIGHT-cam.h, cam.y+fmax(1,int(ms*0.4/scale)));

		ms = SDL_GetTicks() - lastTime;
		mslogic = gamespeed*ms;
		lastTime = SDL_GetTicks();

		gamestate.tracksystem->update(mslogic);
		for(int iTrain=0; iTrain<trains.size(); iTrain++)
			trains[iTrain]->getinput(mslogic);
		for(int iTrain=0; iTrain<trains.size(); iTrain++)
			for(int jTrain=0; jTrain<trains.size(); jTrain++)
				if(iTrain!=jTrain)
					trains[iTrain]->checkcollision(mslogic, trains[jTrain].get());
		for(int iTrain=trains.size()-1; iTrain>=0; iTrain--)
			if(trains[iTrain]->wagons.size() == 0)
				trains.erase(trains.begin()+iTrain);
		for(int iTrain=0; iTrain<trains.size(); iTrain++){
			trains[iTrain]->update(mslogic);
		}
		for(auto& wagon : gamestate.wagons)
			wagon->update(mslogic);
		int lastmoney = money;
		for(auto& building : buildings)
			building->update(mslogic);
		gamestate.update(mslogic);
		gamestate.revenue += money-lastmoney;

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		for(int x=0; x<MAP_WIDTH; x+=128){
		for(int y=0; y<MAP_HEIGHT; y+=128){
			SDL_Rect rect = {x,y,128,128};
			rendertexture(fieldtex, &rect);
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
		else
			gamestate.renderroutes();
		for(auto& train : trains)
			train->render();
		rendertext(std::to_string(int(money)) + " Fr", 20, 2*14, {static_cast<Uint8>(127*(money<0)),static_cast<Uint8>(63*(money>=0)),0,0}, false, false);
		rendertext(std::to_string(int(gamestate.time*0.001/60)) + " min", 20, 3*14, {static_cast<Uint8>(127*(money<0)),static_cast<Uint8>(63*(money>=0)),0,0}, false, false);
		rendertext(std::to_string(int(60*float(gamestate.revenue)/float(gamestate.time*0.001/60))) + " Fr/h", 20, 4*14, {static_cast<Uint8>(127*(money<0)),static_cast<Uint8>(63*(money>=0)),0,0}, false, false);
		SDL_GetMouseState(&xMouse, &yMouse);
		SDL_RenderPresent(renderer);
	}

	close();
}
