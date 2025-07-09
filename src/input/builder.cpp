#include<iostream>
#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"


Builder::Builder(InputManager& owner, Game* newgame) : 
        input(owner), 
        game(newgame), 
        tracksystem(game->getgamestate().gettracksystems())
{}

void TrackBuilder::render(Rendering* r)
{
    if(selectednode || trackorigin.x!=0){
        Tracks::Tracksection section;
        if(selectednode)
            section = Tracks::Input::planconstructionto(tracksystem, tracksystem.getnode(selectednode), input.mapmousepos());
        else
            section = Tracks::Input::planconstructionto(tracksystem, trackorigin, input.mapmousepos());
        Tracks::render(section, r, 1);
        int cost = -ceil(Tracks::Input::getcostoftracks(section));
        r->rendertext(std::to_string(cost), input.screenmousepos().x, input.screenmousepos().y-18, {127, 0, 0}, false, false);
        for(auto track: section.tracks)
            delete track;
        for(auto node: section.nodes)
            delete node;
    }
}

void TrackBuilder::leftclickmap(Vec mappos)
{
    Gamestate& gamestate = game->getgamestate();
    Tracks::Tracksystem& tracksystem = gamestate.gettracksystems();

    if(trackorigin.x!=0 || trackorigin.y!=0){
        if(gamestate.money>0){
            Tracks::Tracksection newsection = Tracks::Input::buildat(tracksystem, trackorigin, mappos);
            selectednode = Tracks::Input::selectat(tracksystem, mappos);
            gamestate.money -= Tracks::Input::getcostoftracks(newsection);
            trackorigin = Vec(0,0);
        }
    }
    else if(selectednode){
        if(gamestate.money>0){
            Tracks::Tracksection newsection = Tracks::Input::buildat(tracksystem, tracksystem.getnode(selectednode), mappos);
            selectednode = Tracks::Input::selectat(tracksystem, mappos);
            gamestate.money -= Tracks::Input::getcostoftracks(newsection);
        }
    }
    else{
        selectednode = Tracks::Input::selectat(tracksystem, mappos);
        if(!selectednode){
            trackorigin = mappos;
        }
    }
}

void TrackBuilder::reset()
{
    selectednode = 0;
    trackorigin = Vec(0,0);
}

SignalBuilder::SignalBuilder(InputManager& owner, Game* newgame) : Builder(owner, newgame)
{
    icon.setspritesheet(game->getsprites(), sprites::signal);
}

void SignalBuilder::render(Rendering* r)
{
    Vec mousepos = input.mapmousepos();
    Vec signalpos = Tracks::Input::plansignalat(tracksystem, mousepos);
    if(norm(signalpos-mousepos)<100){
        icon.color = {127,255,127,255};
        icon.render(r, signalpos);
    }
    else{
        icon.color = {255,127,127,255};
        icon.render(r, mousepos);
    }
}

void SignalBuilder::leftclickmap(Vec mousepos)
{
    Gamestate& gamestate = game->getgamestate();
    if(gamestate.money>0){
        Vec signalpos = Tracks::Input::plansignalat(tracksystem, mousepos);
        if(norm(signalpos-mousepos)<100){
            Tracks::Input::buildsignalat(tracksystem, mousepos);
            gamestate.money-=1;
        }
    }
}