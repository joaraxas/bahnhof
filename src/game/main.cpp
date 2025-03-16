#include<iostream>
#include<string>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"

bool nicetracks = true;

int main(int argv, char** args){
	init();
	Game game;
	game.play();
	close();
}
