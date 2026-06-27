#include<iostream>
#include<string>
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"


int main(int argv, char** args){
	init();
	Game game;

	game.getgamestate().randommap();
	game.play();
	close();
}
