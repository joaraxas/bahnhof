#include<iostream>
#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/track/track.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/rollingstock/rollingstock.h"
#include "bahnhof/graphics/rendering.h"

InputManager::InputManager(Game* whatgame){
    game = whatgame;
}

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
        switch(e.type){
            case SDL_QUIT:{
                game->exit();
                break;
            }
            case SDL_MOUSEBUTTONDOWN:{
                Vec mousepos = mapmousepos();
                if(e.button.button == SDL_BUTTON_RIGHT){
                    selectednode = 0;
                    trackorigin = Vec(0,0);
                    placingtrack = false;
                    placingsignal = false;
                    if(editingroute){
                        Order* neworder = Tracks::Input::generateorderat(tracksystem, mousepos);
                        if(neworder)
                            editingroute->insertorderatselected(neworder);
                    }
                }
                if(e.button.button == SDL_BUTTON_MIDDLE){
                    Tracks::Input::deleteat(tracksystem, mousepos);
                }
                if(e.button.button == SDL_BUTTON_LEFT){
                    if(ui.leftclick(screenmousepos())){}
                    else if(placingsignal){
                        if(gamestate.money>0){
                            Tracks::Input::buildsignalat(tracksystem, mousepos);
                            gamestate.money-=1;
                        }
                        placingsignal = false;
                    }
                    else if(placingtrack){
                        if(trackorigin.x!=0 || trackorigin.y!=0){
                            if(gamestate.money>0){
                                Tracks::Tracksection newsection = Tracks::Input::buildat(tracksystem, trackorigin, mousepos);
                                selectednode = Tracks::Input::selectat(tracksystem, mousepos);
                                gamestate.money -= Tracks::Input::getcostoftracks(newsection);
                            }
                            trackorigin = Vec(0,0);
                        }
                        else if(selectednode){
                            if(gamestate.money>0){
                                Tracks::Tracksection newsection = Tracks::Input::buildat(tracksystem, tracksystem.getnode(selectednode), mousepos);
                                selectednode = Tracks::Input::selectat(tracksystem, mousepos);
                                gamestate.money -= Tracks::Input::getcostoftracks(newsection);
                            }
                        }
                        else{
                            selectednode = Tracks::Input::selectat(tracksystem, mousepos);
                            if(!selectednode){
                                trackorigin = mousepos;
                            }
                        }
                    }
                    else{
                        Train* clickedtrain = gamestate.gettrainmanager().gettrainatpos(mousepos);
                        if(clickedtrain){
                            selecttrain(clickedtrain);
                        }
                        else{
                            if(!Tracks::Input::switchat(tracksystem, mousepos)){
                                selecttrain(nullptr);
                            }
                        }
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:{
                if(e.button.button == SDL_BUTTON_LEFT){
                    Vec mousepos = screenmousepos();
                    ui.leftbuttonup(mousepos);
                }
            }
            case SDL_KEYDOWN:{
                if(e.key.keysym.sym == SDLK_UP){
                    if(editingroute)
                        editingroute->selectedorderid = editingroute->previousorder(editingroute->selectedorderid);
                }
                if(e.key.keysym.sym == SDLK_DOWN){
                    if(editingroute)
                        editingroute->selectedorderid = editingroute->nextorder(editingroute->selectedorderid);
                }
                if(selectedtrain){
                    if(e.key.keysym.sym == SDLK_p){
                        selectedtrain->proceed();
                    }
                }
                if(e.key.keysym.sym == SDLK_n){
                    nicetracks = !nicetracks;
                }
                if(gamestate.money>0){
                    TrainManager& trainmanager = gamestate.gettrainmanager();
                    if(e.key.keysym.sym == SDLK_o){
                        trainmanager.addwagon(new Openwagon(&tracksystem, gamestate.newwagonstate));
                        gamestate.newwagonstate = Tracks::travel(tracksystem, gamestate.newwagonstate, 60);
                        trainmanager.addtrainstoorphans();
                        gamestate.money -= 3;
                    }
                    if(e.key.keysym.sym == SDLK_q){
                        trainmanager.addwagon(new Tankwagon(&tracksystem, gamestate.newwagonstate));
                        gamestate.newwagonstate = Tracks::travel(tracksystem, gamestate.newwagonstate, 72);
                        trainmanager.addtrainstoorphans();
                        gamestate.money -= 3;
                    }
                    if(e.key.keysym.sym == SDLK_y){
                        trainmanager.addwagon(new Locomotive(&tracksystem, gamestate.newwagonstate));
                        gamestate.newwagonstate = Tracks::travel(tracksystem, gamestate.newwagonstate, 60);
                        trainmanager.addtrainstoorphans();
                        gamestate.money -= 8;
                    }
                }
                break;
            }
            case SDL_MOUSEWHEEL:{
                if(e.wheel.y > 0){
                    cam.zoomin(screenmousepos());
                }
                if(e.wheel.y < 0){
                    cam.zoomout(screenmousepos());
                }
                break;
            }
        }
    }

    if(keyispressed(leftpanbutton))
        cam.pan(Vec(-ms, 0));
    if(keyispressed(rightpanbutton))
        cam.pan(Vec(+ms, 0));
    if(keyispressed(uppanbutton))
        cam.pan(Vec(0, -ms));
    if(keyispressed(downpanbutton))
        cam.pan(Vec(0, +ms));

    trainmanager.getinput(this, mslogic);
}

void InputManager::render(Rendering* r, Tracks::Tracksystem& tracksystem)
{
    if(placingtrack)
    if(selectednode || trackorigin.x!=0){
        Tracks::Tracksection section;
        if(selectednode)
            section = Tracks::Input::planconstructionto(tracksystem, tracksystem.getnode(selectednode), mapmousepos());
        else
            section = Tracks::Input::planconstructionto(tracksystem, trackorigin, mapmousepos());
        Tracks::render(section, r, 1);
        int cost = -ceil(Tracks::Input::getcostoftracks(section));
        r->rendertext(std::to_string(cost), screenmousepos().x, screenmousepos().y-18, {127, 0, 0}, false, false);
        for(auto track: section.tracks)
            delete track;
        for(auto node: section.nodes)
            delete node;
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
    int currentmousex, currentmousey;
    float logicalmousex, logicalmousey;
	SDL_GetMouseState(&currentmousex, &currentmousey);
    SDL_RenderWindowToLogical(renderer, currentmousex, currentmousey, &logicalmousex, &logicalmousey);
    return Vec(int(logicalmousex), int(logicalmousey));
}

bool InputManager::keyispressed(const int scancode)
{
	keys = SDL_GetKeyboardState(nullptr);
    return keys[scancode];
}

void InputManager::selecttrain(Train* whattrain)
{
    Gamestate& gamestate = game->getgamestate();
	gamestate.gettrainmanager().deselectall();
	if(whattrain){
		whattrain->selected = true;
    }
	selectedtrain = whattrain;
}

void InputManager::editroute(Route* route)
{
    editingroute = route;
}

void InputManager::placesignal()
{
    placingsignal = true;
    placingtrack = false;
    selecttrain(nullptr);
}


void InputManager::placetrack()
{
    placingtrack = true;
    placingsignal = false;
    selecttrain(nullptr);
};