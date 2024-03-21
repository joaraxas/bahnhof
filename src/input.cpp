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
    Gamestate* gamestate = game->gamestate;
    Tracksystem* tracksystem = gamestate->tracksystem.get();
    RouteManager* routing = gamestate->routing;
    SpriteManager* allsprites = game->allsprites;
    while(SDL_PollEvent(&e)){
        switch(e.type){
            case SDL_QUIT:{
                game->exit();
                break;
            }
            case SDL_MOUSEBUTTONDOWN:{
                Vec mousepos = mapmousepos();
                if(e.button.button == SDL_BUTTON_RIGHT){
                    tracksystem->selectednode = 0;
                    tracksystem->placingsignal = false;
                    if(routing->selectedroute){
                        Order* neworder = tracksystem->generateorderat(mousepos);
                        if(neworder)
                            routing->selectedroute->insertorderatselected(neworder);
                    }
                }
                if(e.button.button == SDL_BUTTON_LEFT){
                    if(tracksystem->selectednode || tracksystem->placingsignal){
                        if(gamestate->money>0)
                            gamestate->money-=tracksystem->buildat(mousepos);
                    }
                    else{
                        Train* clickedtrain = nullptr;
                        for(auto& train : gamestate->trains){
                            for(auto& wagon : train->wagons){
                                if(norm(mousepos-wagon->pos)<wagon->w/2/game->rendering->getscale()){
                                    routing->selectedroute = train->route;
                                    clickedtrain = train.get();
                                }
                                if(clickedtrain) break;
                            }
                        }
                        if(clickedtrain){
                            for(auto& train : gamestate->trains)
                                train->selected = false;
                            clickedtrain->selected = true;
                        }
                        else{
                            if(!gamestate->tracksystem->switchat(mousepos)){
                                gamestate->tracksystem->selectat(mousepos);
                                for(auto& train : gamestate->trains)
                                    train->selected = false;
                                routing->selectedroute = nullptr;
                            }
                        }
                    }
                }
                break;
            }
            case SDL_KEYDOWN:{
                if(e.key.keysym.sym == SDLK_r){
                    if(!routing->selectedroute)
                        routing->addroute();
                }
                if(keyispressed(routeassignbutton)){
                    if(e.key.keysym.sym >= SDLK_1 && e.key.keysym.sym <= SDLK_0+routing->routes.size()){
                        for(auto& train : gamestate->trains)
                            if(train->selected){
                                routing->selectedroute = routing->routes[e.key.keysym.sym-SDLK_0-1].get();
                                train->route = routing->selectedroute;
                            }
                    }
                }
                if(e.key.keysym.sym == SDLK_UP){
                    if(routing->selectedroute)
                        routing->selectedroute->selectedorderid = routing->selectedroute->previousorder(routing->selectedroute->selectedorderid);
                }
                if(e.key.keysym.sym == SDLK_DOWN){
                    if(routing->selectedroute)
                        routing->selectedroute->selectedorderid = routing->selectedroute->nextorder(routing->selectedroute->selectedorderid);
                }
                if(e.key.keysym.sym == SDLK_BACKSPACE){
                    if(routing->selectedroute)
                        routing->selectedroute->removeselectedorder();
                }
                if(e.key.keysym.sym == SDLK_t){
                    if(routing->selectedroute)
                        routing->selectedroute->insertorderatselected(new Turn());
                }
                if(e.key.keysym.sym == SDLK_e){
                    if(routing->selectedroute)
                        routing->selectedroute->insertorderatselected(new Decouple());
                }
                if(e.key.keysym.sym == SDLK_l){
                    if(routing->selectedroute)
                        routing->selectedroute->insertorderatselected(new Loadresource());
                }
                if(e.key.keysym.sym == SDLK_c){
                    if(routing->selectedroute)
                        routing->selectedroute->insertorderatselected(new Couple());
                }
                if(e.key.keysym.sym == SDLK_p){
                    for(auto& train : gamestate->trains)
                        if(train->selected)
                            train->proceed();
                }
                if(e.key.keysym.sym == SDLK_RETURN){
                    for(auto& train : gamestate->trains)
                        if(train->selected){
                            train->go = !train->go;
                            train->speed = 0;
                        }
                }
                if(e.key.keysym.sym == SDLK_z){
                    gamestate->tracksystem->placingsignal = true;
                }
                if(e.key.keysym.sym == SDLK_n){
                    nicetracks = !nicetracks;
                }
                if(gamestate->money>0){
                    if(e.key.keysym.sym == SDLK_o){
                        gamestate->wagons.emplace_back(new Openwagon(gamestate->tracksystem.get(), gamestate->newwagonstate, allsprites));
                        gamestate->newwagonstate = gamestate->tracksystem->travel(gamestate->newwagonstate, 60);
                        gamestate->addtrainstoorphans();
                        gamestate->money -= 3;
                    }
                    if(e.key.keysym.sym == SDLK_q){
                        gamestate->wagons.emplace_back(new Tankwagon(gamestate->tracksystem.get(), gamestate->newwagonstate, allsprites));
                        gamestate->newwagonstate = gamestate->tracksystem->travel(gamestate->newwagonstate, 72);
                        gamestate->addtrainstoorphans();
                        gamestate->money -= 3;
                    }
                    if(e.key.keysym.sym == SDLK_y){
                        gamestate->wagons.emplace_back(new Locomotive(gamestate->tracksystem.get(), gamestate->newwagonstate, allsprites));
                        gamestate->newwagonstate = gamestate->tracksystem->travel(gamestate->newwagonstate, 60);
                        gamestate->addtrainstoorphans();
                        gamestate->money -= 8;
                    }
                }
                break;
            }
            case SDL_MOUSEWHEEL:{
                if(e.wheel.y > 0){
                    game->cam->zoomin(screenmousepos());
                }
                if(e.wheel.y < 0){
                    game->cam->zoomout(screenmousepos());
                }
                break;
            }
        }
    }

    if(keyispressed(leftpanbutton))
        game->cam->pan(Vec(-ms*0.4, 0));
    if(keyispressed(rightpanbutton))
        game->cam->pan(Vec(+ms*0.4, 0));
    if(keyispressed(uppanbutton))
        game->cam->pan(Vec(0, -ms*0.4));
    if(keyispressed(downpanbutton))
        game->cam->pan(Vec(0, +ms*0.4));

    for(int iTrain=0; iTrain<gamestate->trains.size(); iTrain++)
        gamestate->trains[iTrain]->getinput(this, mslogic);
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