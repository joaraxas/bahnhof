#include<iostream>
#include "bahnhof/common/input.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/track.h"
#include "bahnhof/routing/routing.h"
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
    SpriteManager& allsprites = game->getsprites();
    Camera& cam = game->getcamera();
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
                    placingsignal = false;
                    if(routing.selectedroute){
                        Order* neworder = Tracks::Input::generateorderat(tracksystem, mousepos);
                        if(neworder)
                            routing.selectedroute->insertorderatselected(neworder);
                    }
                }
                if(e.button.button == SDL_BUTTON_LEFT){
                    if(placingsignal){
                        if(gamestate.money>0){
                            Tracks::Input::buildsignalat(tracksystem, mousepos);
                            gamestate.money-=1;
                        }
                        placingsignal = false;
                    }
                    else if(selectednode){
                        if(gamestate.money>0){
                            Tracks::Tracksection newsection = Tracks::Input::buildat(tracksystem, tracksystem.getnode(selectednode), mousepos);
	                        selectednode = Tracks::Input::selectat(tracksystem, mousepos);
                            gamestate.money-=Tracks::Input::getcostoftracks(newsection);
                        }
                    }
                    else{
                        Train* clickedtrain = nullptr;
                        for(auto& train : gamestate.trains){
                            for(auto& wagon : train->wagons){
                                if(norm(mousepos-wagon->pos)<wagon->w/2/game->getrendering().getscale()){
                                    routing.selectedroute = train->route;
                                    clickedtrain = train.get();
                                }
                                if(clickedtrain) break;
                            }
                        }
                        if(clickedtrain){
                            selecttrain(clickedtrain);
                        }
                        else{
                            if(!Tracks::Input::switchat(tracksystem, mousepos)){
                                selectednode = Tracks::Input::selectat(tracksystem, mousepos);
                                selecttrain(nullptr);
                                routing.selectedroute = nullptr;
                            }
                        }
                    }
                }
                break;
            }
            case SDL_KEYDOWN:{
                if(e.key.keysym.sym == SDLK_r){
                    if(!routing.selectedroute)
                        routing.addroute();
                }
                if(e.key.keysym.sym == SDLK_UP){
                    if(routing.selectedroute)
                        routing.selectedroute->selectedorderid = routing.selectedroute->previousorder(routing.selectedroute->selectedorderid);
                }
                if(e.key.keysym.sym == SDLK_DOWN){
                    if(routing.selectedroute)
                        routing.selectedroute->selectedorderid = routing.selectedroute->nextorder(routing.selectedroute->selectedorderid);
                }
                if(e.key.keysym.sym == SDLK_BACKSPACE){
                    if(routing.selectedroute)
                        routing.selectedroute->removeselectedorder();
                }
                if(e.key.keysym.sym == SDLK_t){
                    if(routing.selectedroute)
                        routing.selectedroute->insertorderatselected(new Turn());
                }
                if(e.key.keysym.sym == SDLK_e){
                    if(routing.selectedroute)
                        routing.selectedroute->insertorderatselected(new Decouple());
                }
                if(e.key.keysym.sym == SDLK_l){
                    if(routing.selectedroute)
                        routing.selectedroute->insertorderatselected(new Loadresource());
                }
                if(e.key.keysym.sym == SDLK_c){
                    if(routing.selectedroute)
                        routing.selectedroute->insertorderatselected(new Couple());
                }
                if(selectedtrain){
                    if(keyispressed(routeassignbutton)){
                        if(e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_0+routing.routes.size()){
                            routing.selectedroute = routing.routes[e.key.keysym.sym-SDLK_0-1].get();
                            selectedtrain->route = routing.selectedroute;
                        }
                    }
                    if(e.key.keysym.sym == SDLK_p){
                        selectedtrain->proceed();
                    }
                    if(e.key.keysym.sym == SDLK_RETURN){
                        selectedtrain->go = !selectedtrain->go;
                        selectedtrain->speed = 0;
                    }
                }
                if(e.key.keysym.sym == SDLK_z){
                    placingsignal = true;
                }
                if(e.key.keysym.sym == SDLK_n){
                    nicetracks = !nicetracks;
                }
                if(gamestate.money>0){
                    if(e.key.keysym.sym == SDLK_o){
                        gamestate.wagons.emplace_back(new Openwagon(&tracksystem, gamestate.newwagonstate));
                        gamestate.newwagonstate = Tracks::travel(tracksystem, gamestate.newwagonstate, 60);
                        gamestate.addtrainstoorphans();
                        gamestate.money -= 3;
                    }
                    if(e.key.keysym.sym == SDLK_q){
                        gamestate.wagons.emplace_back(new Tankwagon(&tracksystem, gamestate.newwagonstate));
                        gamestate.newwagonstate = Tracks::travel(tracksystem, gamestate.newwagonstate, 72);
                        gamestate.addtrainstoorphans();
                        gamestate.money -= 3;
                    }
                    if(e.key.keysym.sym == SDLK_y){
                        gamestate.wagons.emplace_back(new Locomotive(&tracksystem, gamestate.newwagonstate));
                        gamestate.newwagonstate = Tracks::travel(tracksystem, gamestate.newwagonstate, 60);
                        gamestate.addtrainstoorphans();
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
        cam.pan(Vec(-ms*0.4, 0));
    if(keyispressed(rightpanbutton))
        cam.pan(Vec(+ms*0.4, 0));
    if(keyispressed(uppanbutton))
        cam.pan(Vec(0, -ms*0.4));
    if(keyispressed(downpanbutton))
        cam.pan(Vec(0, +ms*0.4));

    for(int iTrain=0; iTrain<gamestate.trains.size(); iTrain++)
        gamestate.trains[iTrain]->getinput(this, mslogic);
}

void InputManager::render(Rendering* r, Tracks::Tracksystem& tracksystem)
{
    if(selectednode){
        Tracks::Tracksection section = Tracks::Input::planconstructionto(tracksystem, tracksystem.getnode(selectednode), mapmousepos());
        Tracks::render(section, r, 1);
        int cost = -ceil(Tracks::Input::getcostoftracks(section));
        r->rendertext(std::to_string(cost), screenmousepos().x, screenmousepos().y-18, {127, 0, 0}, false, false);
        for(auto track: section.tracks)
            delete track;
        for(auto node: section.nodes)
            delete node;
    }
}

Vec InputManager::mapmousepos()
{
    Vec mousepos = screenmousepos();
    return game->getcamera().mapcoord(mousepos);
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

void InputManager::selecttrain(Train* whattrain)
{
	for(auto& train: game->getgamestate().trains){
		train->selected = false;
	}
	if(whattrain)
		whattrain->selected = true;
	selectedtrain = whattrain;
}