#include<iostream>
#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/rollingstock/rollingstockmanager.h"


Builder::Builder(InputManager& owner, Game* newgame) : 
    input(owner), 
    game(newgame), 
    tracksystem(game->getgamestate().gettracksystems())
{}

void Builder::render(Rendering* r)
{
    Vec mousepos = input.mapmousepos();
    if(!droppedanchor)
        anchorpoint = mousepos;
    else
        updateangle(mousepos);
}

void Builder::leftclickmap(Vec mappos)
{
    anchorpoint = mappos;
    droppedanchor = true;
}

void Builder::leftreleasedmap(Vec mappos)
{
    if(droppedanchor){
        updateangle(mappos);
        if(canbuild()){
            build();
            game->getgamestate().money-=cost;
        }
        Builder::reset();
    }
}

void Builder::reset()
{
    anchorpoint = Vec(0,0);
    droppedanchor = false;
    angleptr = nullptr;
}

bool Builder::canbuild()
{
    if(game->getgamestate().money<cost)
        return false;
    if(!canfit())
        return false;
    return true;
}

void Builder::updateangle(Vec pos)
{
    Vec diff = pos-anchorpoint;
    angleptr = nullptr;
    if(norm(diff) > 20/game->getcamera().getscale()){
        angle = Angle(atan2(-diff.y, diff.x));
        angleptr = &angle;
    }
}

Tracks::Tracksection TrackBuilder::planconstruction(Vec pos)
{
    if(buildingfromstartpoint()){
        return Tracks::Input::planconstructionto(tracksystem, trackstartpoint, pos, angleptr);
    }
    if(selectednode){
        return Tracks::Input::planconstructionto(tracksystem, tracksystem.getnode(selectednode), pos, angleptr);
    }
    if(selectedstate){
        return Tracks::Input::planconstructionto(tracksystem, selectedstate, pos, angleptr);
    }
    return Tracks::Tracksection();
}

bool TrackBuilder::buildingfromstartpoint()
{
    return (trackstartpoint.x!=0 || trackstartpoint.y!=0);
}

void TrackBuilder::render(Rendering* r)
{
    Builder::render(r);
    Tracks::Tracksection section = planconstruction(anchorpoint);
    cost = ceil(Tracks::Input::getcostoftracks(section));
    TracksDisplayMode mode = TracksDisplayMode::planned;
    if(!canbuild())
        mode = TracksDisplayMode::impossible;
    Tracks::render(section, r, mode);
    Vec screenpoint = game->getcamera().screencoord(anchorpoint);
    r->rendertext(std::to_string(int(cost)), screenpoint.x, screenpoint.y-18, {127, 0, 0}, false, false);
    Tracks::Input::discardsection(section);
}

void TrackBuilder::reset()
{
    Builder::reset();
    trackstartpoint = Vec(0,0);
    selectednode = 0;
    selectedstate = State();
}

bool TrackBuilder::canfit()
{
    bool collision = false;

    Tracks::Tracksection section = planconstruction(anchorpoint);
    collision = game->getgamestate().getbuildingmanager().checkcollision(section);
    Tracks::Input::discardsection(section);

    return !collision;
}

void TrackBuilder::build()
{
    Tracks::Tracksection section = planconstruction(anchorpoint);
    if(section){
        cost = Tracks::Input::getcostoftracks(section);
        Tracks::Input::buildsection(tracksystem, section);
        trackstartpoint = Vec(0,0);
        selectedstate = State();
    }
    selectednode = Tracks::Input::selectnodeat(tracksystem, anchorpoint);
    if(selectednode) return;
    selectedstate = Tracks::Input::getstateat(tracksystem, anchorpoint, 20/game->getcamera().getscale());
    if(selectedstate) return;
    trackstartpoint = anchorpoint;
}

SignalBuilder::SignalBuilder(InputManager& owner, Game* newgame) : Builder(owner, newgame)
{
    icon.setspritesheet(game->getsprites(), sprites::signal);
    cost = 1;
}

void SignalBuilder::render(Rendering* r)
{
    Builder::render(r);
    if(canbuild()){
        icon.color = {127,255,127,255};
        Vec signalpos = Tracks::Input::plansignalat(tracksystem, anchorpoint);
        icon.render(r, signalpos);
    }
    else{
        icon.color = {255,127,127,255};
        icon.render(r, anchorpoint);
    }
}

bool SignalBuilder::canfit()
{
    Vec signalpos = Tracks::Input::plansignalat(tracksystem, anchorpoint);
    if(norm(signalpos-anchorpoint)<100)
        return true;
    return false;
}

void SignalBuilder::build()
{
    Tracks::Input::buildsignalat(tracksystem, anchorpoint);
}

void BuildingBuilder::render(Rendering* r)
{
    Builder::render(r);
    if(building){ // this should always be true
        if(building->id==wagonfactory){
            Angle newangle = angle;
            Tracks::Tracksection section = Tracks::Input::planconstructionto(tracksystem, anchorpoint, 600, newangle);
            TracksDisplayMode mode = TracksDisplayMode::planned;
            if(!canbuild())
                mode = TracksDisplayMode::impossible;
            Tracks::render(section, r, mode);
            Tracks::Input::discardsection(section);
        }
        std::unique_ptr<Shape> shape = getplacementat(anchorpoint);
        SDL_Color color;
        if(canbuild()){
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
    angle = Angle(0);
}

void BuildingBuilder::setbuildingtype(const BuildingType& type)
{
    building = &type;
    cost = building->cost;
}

bool BuildingBuilder::canfit()
{
    std::unique_ptr<Shape> shape = getplacementat(anchorpoint);
    if(buildingmanager.checkcollision(*shape.get()))
        return false;
    if(building->id == wagonfactory){
        Angle newangle = angle;
        Tracks::Tracksection section = Tracks::Input::planconstructionto(tracksystem, anchorpoint, 600, newangle);
        if(buildingmanager.checkcollision(section)){
            Tracks::Input::discardsection(section); 
            return false;
        }
        Tracks::Input::discardsection(section); 
    }
    return true;
}

void BuildingBuilder::build()
{
    if(!building){
        std::cout<<"error: no building selected at build!";
        return;
    }
    std::unique_ptr<Shape> shape = getplacementat(anchorpoint);
    switch(building->id)
    {
    case brewery:
        buildingmanager.addbuilding(std::make_unique<Brewery>(game, std::move(shape)));
        break;
    case hopsfield:
        buildingmanager.addbuilding(std::make_unique<Hopsfield>(game, std::move(shape)));
        break;
    case barleyfield:
        buildingmanager.addbuilding(std::make_unique<Barleyfield>(game, std::move(shape)));
        break;
    case city:
        buildingmanager.addbuilding(std::make_unique<City>(game, std::move(shape)));
        break;
    case wagonfactory:{
        RollingStockManager& r = game->getgamestate().getrollingstockmanager();
        Angle newangle = angle;
        Tracks::Tracksection section = Tracks::Input::planconstructionto(tracksystem, anchorpoint, 600, newangle);
        Tracks::Input::buildsection(tracksystem, section);
        State midpointstate = Tracks::getstartpointstate(section);
        midpointstate = flipstate(Tracks::travel(tracksystem, midpointstate, 500));
        buildingmanager.addbuilding(std::make_unique<WagonFactory>(game, std::move(shape), midpointstate, r));
        break;
    }
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
        Angle newangle = angle;
        State neareststate = Tracks::Input::getendpointat(tracksystem, pos, 20);
        Vec buildingmidpoint = Tracks::gettrackextension(tracksystem, neareststate, 500, newangle);
        if(neareststate)
            return std::make_unique<RotatedRectangle>(
                Tracks::getpos(tracksystem, neareststate) + buildingmidpoint, 
                building->size.x, 
                building->size.y, 
                newangle
            );
        else
            return std::make_unique<RotatedRectangle>(
                pos + buildingmidpoint, 
                building->size.x, 
                building->size.y, 
                newangle
            );
    }
    case city:{
        float ang = 120*pi/180;
        Vec otherpos = globalcoords(Vec{100*sin(ang),-100*(1-cos(ang))}, angle, pos);
        return std::make_unique<AnnularSector>(pos, angle, otherpos, 20);
    }
    default:{
        if(angle==Angle(0))
            return std::make_unique<Rectangle>(pos, building->size);
        return std::make_unique<RotatedRectangle>(pos, building->size.x, building->size.y, angle);
    }
    }
}
