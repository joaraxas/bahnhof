#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"
#include "bahnhof/track/state.h"
#include "bahnhof/graphics/sprite.h"

namespace Tracks{
    class Tracksystem;}

class Game;
class Rendering;
class Train;
class Route;
namespace UI{
    class EditableText;
}

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

class InputManager;

class TextInputManager
{
public:
    TextInputManager(InputManager& owner) : input(owner) {};
    void starttextinput(UI::EditableText* textobject);
    void savetext();
    void endtextinput();
    bool handle(SDL_Event& e);
    bool iswriting() {return editingtextobject!=nullptr;};
private:
    InputManager& input;
    UI::EditableText* editingtextobject = nullptr;
};

class TrackBuilder
{
public:
    TrackBuilder(InputManager& owner, Game* newgame);
    void render(Rendering*);
    void leftclickmap(Vec mappos);
    void reset();
private:
    Game* game;
    InputManager& input;
    Tracks::Tracksystem& tracksystem;
    nodeid selectednode = 0;
    Vec trackorigin{0,0};
};

class SignalBuilder
{
public:
    SignalBuilder(InputManager& owner, Game* newgame);
    void render(Rendering*);
    void leftclickmap(Vec mappos);
    void reset();
private:
    Game* game;
    InputManager& input;
    Tracks::Tracksystem& tracksystem;
    Icon icon;
};

enum InputState {
    idle,
    placingsignals,
    placingtracks,
    placingbuildings
};

class InputManager
{
public:
    InputManager(Game* whatgame);
    TextInputManager& gettextinputmanager() {return textinput;};
    void handle(int ms, int mslogic);
    void render(Rendering*);
    Vec screenmousepos();
    Vec mapmousepos();
    bool iskeypressed(const int scancode);
    bool isleftmousepressed();
    void editroute(Route* route);
    void placesignal();
    void placetrack();
private:
    void resetinput();
    void leftclickmap(Vec mousepos);
    void rightclickmap(Vec mousepos);
    void keydown(SDL_Keycode key);
    void selecttrain(Train* train);
    Game* game;
    TextInputManager textinput;
    TrackBuilder trackbuilder;
    SignalBuilder signalbuilder;
    InputState inputstate = idle;
    const Uint8* keys;
    Route* editingroute = nullptr;
};