#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"

InputManager::InputManager(Game* whatgame){
    game = whatgame;
}

Vec InputManager::mapmousepos()
{
    Vec mousepos = screenmousepos();
    return game->cam->mapcoord(mousepos);
}

Vec InputManager::screenmousepos()
{
    int currentmousex, currentmousey;
	SDL_GetMouseState(&currentmousex, &currentmousey);
    return Vec(currentmousex, currentmousey);
}

bool InputManager::keyispressed(const int scancode)
{
	keys = SDL_GetKeyboardState(nullptr);
    return keys[scancode];
}