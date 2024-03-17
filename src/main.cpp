#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/rendering.h"
#include "bahnhof/common/input.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/timing.h"

bool nicetracks = true;

int main(){
	init();
	Game game;
	Gamestate* gamestate = game.gamestate;
	InputManager* input = game.input;
	Rendering* rendering = game.rendering;
	TimeManager* timer = game.timer;

	while(!game.quit){
		timer->tick();

		input->handle(timer->getms(), timer->getmslogic());
		
		gamestate->update(timer->getmslogic());

		rendering->render();
	}

	close();
}
