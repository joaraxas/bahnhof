#include "bahnhof/input/builder.h"
#include "bahnhof/common/shape.h"
#include "bahnhof/common/geometry.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"

BuildingBuilder::BuildingBuilder(InputManager& i, Game* g, const BuildingType& b) : 
    Builder(i, g), 
    buildingmanager(g->getgamestate().getbuildingmanager()),
    building(b)
{
    cost = building.cost;
}

void BuildingBuilder::render(Rendering* r)
{
    Builder::render(r);
    
    if(building.id==wagonfactory){
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
        color = building.color;
        color.a = 127;
    }
    else{
        color = {127, 0, 0, 127};
    }
    shape.get()->renderfilled(r, color);
}

void BuildingBuilder::reset()
{
    Builder::reset();
    angle = Angle::zero;
}

bool BuildingBuilder::canfit()
{
    std::unique_ptr<Shape> shape = getplacementat(anchorpoint);
    if(buildingmanager.checkcollision(*shape.get()))
        return false;
    if(building.id == wagonfactory){
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
    std::unique_ptr<Shape> shape = getplacementat(anchorpoint);
    switch(building.id)
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
        State midpointstate = Tracks::Input::getstateat(tracksystem, anchorpoint);
        if(!Tracks::getorientation(tracksystem, midpointstate).isbetween(newangle-Angle(1), newangle+Angle(1)))
            midpointstate.alignedwithtrack = !midpointstate.alignedwithtrack;
        midpointstate = flipstate(Tracks::travel(tracksystem, midpointstate, 500));
        buildingmanager.addbuilding(std::make_unique<WagonFactory>(game, std::move(shape), midpointstate, r));
        break;
    }
    default:
        std::cout<<"error: building id "<<building.id<<" is not covered by BuildingBuilder::build!";
        break;
    }
}

std::unique_ptr<Shape> BuildingBuilder::getplacementat(Vec pos) const
{
    switch (building.id)
    {
    case wagonfactory:{
        Angle newangle = angle;
        State neareststate = Tracks::Input::getendpointat(tracksystem, pos, 20);
        Vec buildingmidpoint = Tracks::gettrackextension(tracksystem, neareststate, 500, newangle);
        if(neareststate)
            return std::make_unique<RotatedRectangle>(
                Tracks::getpos(tracksystem, neareststate) + buildingmidpoint, 
                building.size.x, 
                building.size.y, 
                newangle
            );
        else
            return std::make_unique<RotatedRectangle>(
                pos + buildingmidpoint, 
                building.size.x, 
                building.size.y, 
                newangle
            );
    }
    default:{
        if(angle==Angle::zero)
            return std::make_unique<Rectangle>(pos, building.size);
        return std::make_unique<RotatedRectangle>(pos, building.size.x, building.size.y, angle);
    }
    }
}
