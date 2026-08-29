#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/economy/money.h"


SignalBuilder::SignalBuilder(InputManager& owner, Game* newgame) : Builder(owner, newgame)
{
    icon.setspritesheet(game->getsprites(), sprites::signal);
    cost = 1_Fr;
}

void SignalBuilder::render(Rendering* r)
{
    Builder::render(r);
    if(canbuild()){
        icon.color = {127,255,127,255};
        Vec signalpos = Tracks::Input::plansignalat(tracksystem, orientor.getanchor());
        icon.render(r, signalpos);
    }
    else{
        icon.color = {255,127,127,255};
        icon.render(r, orientor.getanchor());
    }
}

bool SignalBuilder::canfit()
{
    Vec signalpos = Tracks::Input::plansignalat(tracksystem, orientor.getanchor());
    if(norm(signalpos-orientor.getanchor())<100)
        return true;
    return false;
}

void SignalBuilder::build()
{
    Tracks::Input::buildsignalat(tracksystem, orientor.getanchor());
}
