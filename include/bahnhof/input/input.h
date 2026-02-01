#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "bahnhof/ui/ownership.h"
#include "bahnhof/track/state.h"
#include "bahnhof/graphics/sprite.h"

namespace Tracks{
    class Tracksystem;}

class Game;
class Rendering;
class Train;
class Route;
struct BuildingType;

const int gasbutton = SDL_SCANCODE_RIGHT;
const int brakebutton = SDL_SCANCODE_LEFT;
const int gearbutton = SDL_SCANCODE_LSHIFT;
const int numberbuttons[10] = {SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9};
const int loadbutton = SDL_SCANCODE_L;
const int unloadbutton = SDL_SCANCODE_U;
const int couplebutton = SDL_SCANCODE_G;
const int leftpanbutton = SDL_SCANCODE_A;
const int rightpanbutton = SDL_SCANCODE_D;
const int uppanbutton = SDL_SCANCODE_W;
const int downpanbutton = SDL_SCANCODE_S;

class TextInputManager;
class InputMode;
class TrackBuilder;
class SignalBuilder;
class BuildingBuilder;

class InputManager
{
public:
    InputManager(Game* whatgame);
    ~InputManager();
    TextInputManager& gettextinputmanager();
    void handle(int ms, int mslogic);
    void render(Rendering*);
    Vec screenmousepos();
    Vec mapmousepos();
    bool iskeypressed(const int scancode);
    bool isleftmousepressed();
    void resetinput();
    void setinputmode(std::unique_ptr<InputMode> m);
private:
    void keydown(SDL_Keycode key);
    Game* game;
    std::unique_ptr<TextInputManager> textinput;
    std::unique_ptr<InputMode> mode;
    const Uint8* keys;
};