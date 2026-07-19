#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "textinput.h"
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

namespace Keynames{
    static constexpr int numberbuttons[10] = {SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9};
    static constexpr int fastpanbutton = SDL_SCANCODE_LSHIFT;
    static constexpr int leftpanbutton = SDL_SCANCODE_A;
    static constexpr int rightpanbutton = SDL_SCANCODE_D;
    static constexpr int uppanbutton = SDL_SCANCODE_W;
    static constexpr int downpanbutton = SDL_SCANCODE_S;
}

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
    void render(Rendering*) const;
    Vec mapmousepos() const;
    bool iskeypressed(const int scancode) const;
    bool isleftmousepressed() const;
    void resetinput();
    void setinputmode(std::unique_ptr<InputMode> m);
private:
    void onkeydown(SDL_Keycode key);
    Game* game;
    TextInputManager textinput;
    std::unique_ptr<InputMode> mode;
};

Vec screenmousepos();