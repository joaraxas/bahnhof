#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/buildings/buildingtypes.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/buildings/buildingmanager.h"


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
    angle = Angle::zero;
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
        Angle ang(120*pi/180);
        return std::make_unique<AnnularSector>(pos, angle, ang, 100, 20);
    }
    default:{
        if(angle==Angle::zero)
            return std::make_unique<Rectangle>(pos, building->size);
        return std::make_unique<RotatedRectangle>(pos, building->size.x, building->size.y, angle);
    }
    }
}
