#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/buildings/buildingmanager.h"


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
