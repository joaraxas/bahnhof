#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"


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
