#include "bahnhof/input/input.h"
#include "bahnhof/input/textinput.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/track/track.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/rollingstock/rollingstockmanager.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/graphics/rendering.h"

InputManager::InputManager(Game* whatgame) : 
        game(whatgame),
        textinput(std::make_unique<TextInputManager>(*this)),
        trackbuilder(std::make_unique<TrackBuilder>(*this, game)),
        signalbuilder(std::make_unique<SignalBuilder>(*this, game)),
        builder(std::make_unique<BuildingBuilder>(*this, game))
{}

InputManager::~InputManager() {}

TextInputManager& InputManager::gettextinputmanager()
{
    return *textinput;
};

void InputManager::handle(int ms, int mslogic){
	SDL_Event e;
    Gamestate& gamestate = game->getgamestate();
    Tracks::Tracksystem& tracksystem = gamestate.gettracksystems();
    RouteManager& routing = gamestate.getrouting();
    TrainManager& trainmanager = gamestate.gettrainmanager();
    SpriteManager& allsprites = game->getsprites();
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
                rightclickmap(mousepos);
            }
            if(e.button.button == SDL_BUTTON_MIDDLE){
                resetinput();
                Tracks::Input::deleteat(tracksystem, mousepos);
            }
            if(e.button.button == SDL_BUTTON_LEFT){
                leftclickmap(mousepos);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP:{
            if(e.button.button == SDL_BUTTON_LEFT){
                ui.leftbuttonup(screenmousepos());
                leftreleasedmap(mapmousepos());
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

void InputManager::leftclickmap(Vec mousepos)
{
    switch (inputstate)
    {
    case placingsignals:
        signalbuilder->leftclickmap(mousepos);
        break;
    
    case placingtracks:
        trackbuilder->leftclickmap(mousepos);
        break;
    
    case placingbuildings:
        builder->leftclickmap(mousepos);
        break;
   
    case idle:{
        Gamestate& gamestate = game->getgamestate();
        if(gamestate.getbuildingmanager().leftclick(mousepos))
            break;
        
        Train* clickedtrain = gamestate.gettrainmanager().gettrainatpos(mousepos);
        if(clickedtrain){
            selecttrain(clickedtrain);
            break;
        }
        
        Tracks::Tracksystem& tracksystem = gamestate.gettracksystems();
        if(Tracks::Input::switchat(tracksystem, mousepos)){
            break;
        }

        selecttrain(nullptr);
        break;
    }
    
    default:
        std::cout<<"warning, input state "<<inputstate<<"not covered by InputManager";
        break;
    }
}

void InputManager::rightclickmap(Vec mousepos)
{
    Gamestate& gamestate = game->getgamestate();
    Tracks::Tracksystem& tracksystem = gamestate.gettracksystems();
    resetinput();
    if(editingroute){
        Order* neworder = Tracks::Input::generateorderat(tracksystem, mousepos);
        if(neworder)
            editingroute->insertorderatselected(neworder);
    }
}

void InputManager::leftreleasedmap(Vec mousepos)
{
    switch (inputstate)
    {
    case placingsignals:
        signalbuilder->leftreleasedmap(mousepos);
        break;
    
    case placingtracks:
        trackbuilder->leftreleasedmap(mousepos);
        break;
    
    case placingbuildings:
        builder->leftreleasedmap(mousepos);
        break;
    
    case idle:
        break;
    
    default:
        std::cout<<"warning, input state "<<inputstate<<"not covered by InputManager at left mouse release";
        break;
    }
}

void InputManager::keydown(SDL_Keycode key)
{
    Gamestate& gamestate = game->getgamestate();
    Tracks::Tracksystem& tracksystem = gamestate.gettracksystems();
    TrainManager& trainmanager = gamestate.gettrainmanager();
    RollingStockManager& rollingstock = gamestate.getrollingstockmanager();
    switch (key)
    {
    case SDLK_n:
        nicetracks = !nicetracks;
        break;
    
    default:
        break;
    }
}

void InputManager::render(Rendering* r)
{
    switch (inputstate)
    {
    case placingtracks:
        trackbuilder->render(r);
        break;

    case placingsignals:
        signalbuilder->render(r);
        break;

    case placingbuildings:
        builder->render(r);
        break;
    }
    if(editingroute)
        editingroute->render(r);
}

Vec InputManager::mapmousepos()
{
    Vec mousepos = screenmousepos();
    return game->getcamera().mapcoord(mousepos);
}

Vec InputManager::screenmousepos()
{
    // use this function instead of SDL_GetMouseState to get mouse position in useful logical pixels
    int currentmousex, currentmousey;
    float logicalmousex, logicalmousey;
	SDL_GetMouseState(&currentmousex, &currentmousey);
    SDL_RenderWindowToLogical(renderer, currentmousex, currentmousey, &logicalmousex, &logicalmousey);
    return Vec(int(logicalmousex), int(logicalmousey));
}

bool InputManager::iskeypressed(const int scancode)
{
	keys = SDL_GetKeyboardState(nullptr);
    return keys[scancode];
}

bool InputManager::isleftmousepressed()
{
    return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK);
}

void InputManager::selecttrain(Train* whattrain)
{
    Gamestate& gamestate = game->getgamestate();
	gamestate.gettrainmanager().deselectall();
	if(whattrain){
		whattrain->select();
    }
}

void InputManager::editroute(Route* route)
{
    editingroute = route;
}

void InputManager::placesignal()
{
    resetinput();
    inputstate = placingsignals;
}

void InputManager::placetrack()
{
    resetinput();
    inputstate = placingtracks;
}

void InputManager::placebuilding(const BuildingType& type)
{
    resetinput();
    inputstate = placingbuildings;
    builder->setbuildingtype(type);
}

void InputManager::resetinput()
{
    trackbuilder->reset();
    signalbuilder->reset();
    builder->reset();
    inputstate = idle;
}
