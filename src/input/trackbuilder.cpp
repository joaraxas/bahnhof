#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/ui/ui.h"

TrackBuilder::TrackBuilder(InputManager& i, Game* g) : 
        Builder(i, g), ui(g->getui()) {};

Tracks::Tracksection TrackBuilder::planconstruction(Vec pos)
{
    Angle* angleptr = nullptr;
    if(angleisset){
        angleptr = &angle;
    }
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

bool TrackBuilder::islayingtrack()
{
    return selectednode || selectedstate || buildingfromstartpoint();
}

void TrackBuilder::render(Rendering* r)
{
    Builder::render(r);
    Tracks::Tracksection section = planconstruction(anchorpoint);
    cost = Tracks::Input::getcostoftracks(section);
    TracksDisplayMode mode = TracksDisplayMode::planned;
    if(!canbuild())
        mode = TracksDisplayMode::impossible;
    Tracks::render(section, r, mode);
    if(islayingtrack()){
        ui.addtooltip(std::string(cost));
        if(!nicetracks){
            ui.addtooltip(std::format("minradius: {:.0f} px",
                Tracks::Input::getminradiusofsection(section)));
            ui.addtooltip(std::to_string(section.tracks.size())+" tracks");
        }
        else{
            if(isinf(Tracks::Input::getminradiusofsection(section))){
                ui.addtooltip("radius many m");
            }
            else
                ui.addtooltip(std::format("radius {:.0f} m",
                    pixelstometers(Tracks::Input::getminradiusofsection(section))));
        }
    }
    else
        ui.addtooltip("Click track startpoint");

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
    if(!islayingtrack())
        return true;
    bool acceptablesection = true;

    Tracks::Tracksection section = planconstruction(anchorpoint);
    acceptablesection &= (Tracks::Input::getminradiusofsection(section) >= 100);
    acceptablesection &= !game->getgamestate().getbuildingmanager().checkcollision(section);
    Tracks::Input::discardsection(section);

    return acceptablesection;
}

void TrackBuilder::build()
{
    if(islayingtrack()){
        Tracks::Tracksection section = planconstruction(anchorpoint);
        if(section){
            cost = Tracks::Input::getcostoftracks(section);
            Tracks::Input::buildsection(tracksystem, section);
            trackstartpoint = Vec(0,0);
            selectedstate = State();
        }
    }
    selectednode = Tracks::Input::selectnodeat(tracksystem, anchorpoint);
    if(selectednode) return;
    selectedstate = Tracks::Input::getstateat(tracksystem, anchorpoint, 20/game->getcamera().getscale());
    if(selectedstate) return;
    trackstartpoint = anchorpoint;
}
