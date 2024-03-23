#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/timing.h"


Game::Game()
{
	gamename = "Name of the game";
	timer = new TimeManager();
	input = new InputManager(this);
	cam = new Camera(this);
	rendering = new Rendering(this, cam);
	allsprites = new SpriteManager();
	resources = new ResourceManager(this);
	gamestate = new Gamestate(this);
	quit = false;
}

Game::~Game()
{
	delete timer;
	delete input;
	delete cam;
	delete rendering;
	delete allsprites;
	delete resources;
	delete gamestate;
}

void Game::play()
{
	while(!quit){
		timer->tick();
		input->handle(timer->getms(), timer->getmslogic());
		gamestate->update(timer->getmslogic());
		rendering->render(gamestate);
	}
}

void Game::exit()
{
	quit = true;
}