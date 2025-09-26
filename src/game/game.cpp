#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/resources/storage.h"
#include "bahnhof/resources/resources.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/timing.h"
#include "bahnhof/ui/ui.h"


Game::Game()
{
	gamename = "Name of the game";
	timer = std::make_unique<TimeManager>();
	cam = std::make_unique<Camera>(this);
	rendering = std::make_unique<Rendering>(this, cam.get());
	ui = std::make_unique<InterfaceManager>(this);
	allsprites = std::make_unique<SpriteManager>(this);
	resources = std::make_unique<ResourceManager>(this);
	gamestate = std::make_unique<Gamestate>(this);
	input = std::make_unique<InputManager>(this);
	quit = false;
}

Game::~Game()
{}

void Game::play()
{
	gamestate->randommap();
	while(!quit){
		timer->tick();
		input->handle(timer->getms(), timer->getmslogic());
		ui->update(timer->getms());
		gamestate->update(timer->getmslogic());
		rendering->render(gamestate.get());
	}
}

void Game::exit()
{
	quit = true;
}