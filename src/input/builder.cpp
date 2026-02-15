#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/economy/company.h"


Builder::Builder(InputManager& owner, Game* newgame) : 
    input(owner), 
    game(newgame), 
    tracksystem(game->getgamestate().gettracksystems()),
    account(game->getgamestate().getmycompany().getaccount())
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

void Builder::rightclickmap(Vec mappos)
{
    input.resetinput();
}

void Builder::leftreleasedmap(Vec mappos)
{
    if(droppedanchor){
        updateangle(mappos);
        if(canbuild()){
            build();
            account.pay(cost);
        }
        Builder::reset();
    }
}

void Builder::reset()
{
    anchorpoint = Vec(0,0);
    droppedanchor = false;
    angleisset = false;
}

bool Builder::canbuild()
{
    if(!account.canafford(cost))
        return false;
    if(!canfit())
        return false;
    return true;
}

void Builder::updateangle(Vec pos)
{
    Vec diff = pos-anchorpoint;
    angleisset = false;
    if(norm(diff) > 20/game->getcamera().getscale()){
        angle = Angle(diff);
        angleisset = true;
    }
}
