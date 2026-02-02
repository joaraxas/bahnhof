#include "bahnhof/input/input.h"
#include "bahnhof/input/textinput.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/rollingstockmanager.h"
#include "bahnhof/graphics/rendering.h"

InputManager::InputManager(Game* whatgame) : 
    game(whatgame),
    textinput(std::make_unique<TextInputManager>(*this)),
    mode(std::make_unique<IdleMode>(*whatgame))
{}

InputManager::~InputManager() {}

TextInputManager& InputManager::gettextinputmanager() const
{
    return *textinput;
};

void InputManager::handle(int ms, int mslogic){
	SDL_Event e;
    Gamestate& gamestate = game->getgamestate();
    TrainManager& trainmanager = gamestate.gettrainmanager();
    Camera& cam = game->getcamera();
    InterfaceManager& ui = game->getui();

    ui.mousehover(screenmousepos(), ms);

    while(SDL_PollEvent(&e)){
        switch(e.type)
        {
        case SDL_QUIT:{
            game->exit();
            break;
        }
        
        case SDL_MOUSEBUTTONDOWN:{
            if(textinput->iswriting()){
                textinput->endtextinput();
            }
            if(ui.click(screenmousepos(), e.button.button))
                break;
            Vec mousepos = mapmousepos();
            if(e.button.button == SDL_BUTTON_RIGHT){
                mode->rightclickmap(mousepos);
            }
            if(e.button.button == SDL_BUTTON_LEFT){
                mode->leftclickmap(mousepos);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP:{
            if(e.button.button == SDL_BUTTON_LEFT){
                ui.leftbuttonup(screenmousepos());
                mode->leftreleasedmap(mapmousepos());
            }
            break;
        }

        case SDL_KEYDOWN:{
            if(textinput->handle(e))
                break;
            keydown(e.key.keysym.sym);
            break;
        }

        case SDL_TEXTINPUT:{
            textinput->handle(e);
            break;
        }

        case SDL_MOUSEWHEEL:{
            if(e.wheel.y > 0){
                if(ui.scroll(screenmousepos(), e.wheel.y))
                    {}
                else
                    cam.zoomin(screenmousepos());
            }
            if(e.wheel.y < 0){
                if(ui.scroll(screenmousepos(), e.wheel.y))
                    {}
                else
                    cam.zoomout(screenmousepos());
            }
            break;
        }

        case SDL_WINDOWEVENT:{
            if(e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                ui.handlewindowsizechange();
            }
            break;
        }
        }
    }

    if(isleftmousepressed())
        ui.leftpressed(screenmousepos(), mslogic);

    if(!textinput->iswriting()){
        if(iskeypressed(leftpanbutton))
            cam.pan(Vec(-ms, 0));
        if(iskeypressed(rightpanbutton))
            cam.pan(Vec(+ms, 0));
        if(iskeypressed(uppanbutton))
            cam.pan(Vec(0, -ms));
        if(iskeypressed(downpanbutton))
            cam.pan(Vec(0, +ms));

        trainmanager.getinput(this, mslogic);
    }
}

void InputManager::keydown(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_n:
        nicetracks = !nicetracks;
        break;
    
    default:
        break;
    }
}

void InputManager::render(Rendering* r) const
{
    mode->render(r);
}

Vec InputManager::mapmousepos() const
{
    Vec mousepos = screenmousepos();
    return game->getcamera().mapcoord(mousepos);
}

Vec InputManager::screenmousepos() const
{
    // use this function instead of SDL_GetMouseState to get mouse position in useful logical pixels
    int currentmousex, currentmousey;
    float logicalmousex, logicalmousey;
	SDL_GetMouseState(&currentmousex, &currentmousey);
    SDL_RenderWindowToLogical(renderer, currentmousex, currentmousey, &logicalmousex, &logicalmousey);
    return Vec(int(logicalmousex), int(logicalmousey));
}

bool InputManager::iskeypressed(const int scancode) const
{
	const Uint8* keys = SDL_GetKeyboardState(nullptr);
    return keys[scancode];
}

bool InputManager::isleftmousepressed() const
{
    return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK);
}

void InputManager::resetinput()
{
    mode = std::make_unique<IdleMode>(*game);
}

void InputManager::setinputmode(std::unique_ptr<InputMode> m)
{
    mode = std::move(m);
}