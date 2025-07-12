#include<iostream>
#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/buildings/buildings.h"


Builder::Builder(InputManager& owner, Game* newgame) : 
        input(owner), 
        game(newgame), 
        tracksystem(game->getgamestate().gettracksystems())
{}

void Builder::leftclickmap(Vec pos)
{
    if(canbuild(pos)){
        build(pos);
        game->getgamestate().money-=cost;
    }
}

bool Builder::canbuild(Vec pos)
{
    if(game->getgamestate().money<cost)
        return false;
    if(!canfit(pos))
        return false;
    return true;
}

void TrackBuilder::render(Rendering* r)
{
    if(selectednode || trackorigin.x!=0){
        Tracks::Tracksection section;
        if(selectednode)
            section = Tracks::Input::planconstructionto(tracksystem, tracksystem.getnode(selectednode), input.mapmousepos());
        else
            section = Tracks::Input::planconstructionto(tracksystem, trackorigin, input.mapmousepos());
        cost = ceil(Tracks::Input::getcostoftracks(section));
        Tracks::render(section, r, 2-canbuild(input.mapmousepos()));
        r->rendertext(std::to_string(int(cost)), input.screenmousepos().x, input.screenmousepos().y-18, {127, 0, 0}, false, false);
        for(auto track: section.tracks)
            delete track;
        for(auto node: section.nodes)
            delete node;
    }
}

void TrackBuilder::build(Vec pos)
{
    if(trackorigin.x!=0 || trackorigin.y!=0){
        Tracks::Tracksection newsection = Tracks::Input::buildat(tracksystem, trackorigin, pos);
        selectednode = Tracks::Input::selectnodeat(tracksystem, pos);
        trackorigin = Vec(0,0);
    }
    else if(selectednode){
        Tracks::Tracksection newsection = Tracks::Input::buildat(tracksystem, tracksystem.getnode(selectednode), pos);
        selectednode = Tracks::Input::selectnodeat(tracksystem, pos);
    }
    else{
        selectednode = Tracks::Input::selectnodeat(tracksystem, pos);
        if(!selectednode){
            trackorigin = pos;
        }
    }
}

void TrackBuilder::reset()
{
    Builder::reset();
    selectednode = 0;
    trackorigin = Vec(0,0);
}

SignalBuilder::SignalBuilder(InputManager& owner, Game* newgame) : Builder(owner, newgame)
{
    icon.setspritesheet(game->getsprites(), sprites::signal);
    cost = 1;
}

void SignalBuilder::render(Rendering* r)
{
    Vec mousepos = input.mapmousepos();
    if(canbuild(mousepos)){
        icon.color = {127,255,127,255};
        Vec signalpos = Tracks::Input::plansignalat(tracksystem, mousepos);
        icon.render(r, signalpos);
    }
    else{
        icon.color = {255,127,127,255};
        icon.render(r, mousepos);
    }
}

bool SignalBuilder::canfit(Vec pos)
{
    Vec signalpos = Tracks::Input::plansignalat(tracksystem, pos);
    if(norm(signalpos-pos)<100)
        return true;
    return false;
}

void SignalBuilder::build(Vec pos)
{
    Tracks::Input::buildsignalat(tracksystem, pos);
}

void BuildingBuilder::render(Rendering* r)
{
    if(building){ // this should always be true
        Vec mousepos = input.mapmousepos();
        std::unique_ptr<Shape> shape = getplacementat(mousepos);
        SDL_Color color;
        if(canbuild(mousepos)){
            color = building->color;
            color.a = 127;
        }
        else{
            color = {127, 0, 0, 127};
        }
        shape.get()->renderfilled(r, color);
    }
    else{
        std::cout<<"error: no building selected when calling BuildingBuilder::render!"<<std::endl;
    }
}

void BuildingBuilder::reset()
{
    Builder::reset();
    building = nullptr;
}

void BuildingBuilder::setbuildingtype(const BuildingType& type)
{
    building = &type;
    cost = building->cost;
}

void BuildingBuilder::build(Vec pos)
{
    if(!building){
        std::cout<<"error: no building selected at build!";
        return;
    }
    std::unique_ptr<Shape> shape = getplacementat(pos);
    switch(building->id)
    {
    case brewery:
        game->getgamestate().buildings.emplace_back(new Brewery(game, std::move(shape)));
        break;
    case hopsfield:
        game->getgamestate().buildings.emplace_back(new Hopsfield(game, std::move(shape)));
        break;
    case barleyfield:
        game->getgamestate().buildings.emplace_back(new Barleyfield(game, std::move(shape)));
        break;
    case city:
        game->getgamestate().buildings.emplace_back(new City(game, std::move(shape)));
        break;
    case wagonfactory:{
        nodeid selectednode = Tracks::Input::selectnodeat(tracksystem, pos);
        if(selectednode)
            game->getgamestate().buildings.emplace_back(new WagonFactory(game, std::move(shape), selectednode));
        else
            game->getgamestate().buildings.emplace_back(new WagonFactory(game, std::move(shape)));
        break;}
    default:
        std::cout<<"error: building id "<<building->id<<" is not covered by BuildingBuilder::build!";
        break;
    }
}

std::unique_ptr<Shape> BuildingBuilder::getplacementat(Vec pos)
{
    switch (building->id)
    {
    case wagonfactory:{
        State neareststate = Tracks::Input::getstateat(tracksystem, pos);
        if(neareststate.track!=0 &&
                norm(pos-Tracks::getpos(tracksystem, neareststate))<40 &&
                Tracks::isendofline(tracksystem, neareststate)){
            float angle = Tracks::getorientation(tracksystem, neareststate);
            Vec statepos = Tracks::getpos(tracksystem, neareststate);
            return std::make_unique<RotatedRectangle>(statepos.x+400*cos(-angle), statepos.y+400*sin(-angle), building->size.x, building->size.y, angle);
        }
        else
            return std::make_unique<RotatedRectangle>(pos.x+400, pos.y, building->size.x, building->size.y);
    }
    
    default:
        return std::make_unique<RotatedRectangle>(pos.x, pos.y, building->size.x, building->size.y, pi/4);
    }
}