#include<iostream>
#include<string>
#include "bahnhof/common/rendering.h"
#include "bahnhof/common/gamestate.h"

bool nicetracks = true;

int main(){
	init();
	Game game;
	game.play();
	close();
}
