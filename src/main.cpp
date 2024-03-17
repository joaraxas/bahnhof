#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/rendering.h"
#include "bahnhof/common/input.h"
#include "bahnhof/common/gamestate.h"

bool nicetracks = true;
int xMouse, yMouse;
float gamespeed = 1;

int main(){
	init();
	Game game;
	Gamestate* gamestate = game.gamestate;
	InputManager* input = game.input;
	Rendering* rendering = game.rendering;
	int ms = 0;
	int mslogic = ms*gamespeed;
	int startTime = SDL_GetTicks();
	int lastTime = SDL_GetTicks();

	while(!game.quit){
		ms = SDL_GetTicks() - lastTime;
		mslogic = gamespeed*ms;
		lastTime = SDL_GetTicks();

		input->handle(ms, mslogic);
		
		gamestate->update(mslogic);

		rendering->render();
	}

	close();
}
