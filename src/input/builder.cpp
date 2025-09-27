#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"


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
        angle = Angle(diff);
        angleptr = &angle;
    }
}
