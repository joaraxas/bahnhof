#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/input/controlmode.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/economy/company.h"


void Orientor::startpoint(Vec mappos)
{
    anchorpoint = mappos;
    droppedanchor = true;
}

void Orientor::endpoint(Vec mappos)
{
    if(droppedanchor)
        updateangle(mappos);
    else
        anchorpoint = mappos;
}

void Orientor::reset()
{
    anchorpoint = Vec(0,0);
    droppedanchor = false;
    valid = false;
}

void Orientor::updateangle(Vec mappos)
{
    Vec diff = mappos-anchorpoint;
    if(norm(diff) > 20/cam.getscale()){
        angle = Angle(diff);
        valid = true;
    }
    else{
        valid = false;
    }
}


Builder::Builder(InputManager& inp, Game* newgame) : 
    game(newgame),
    tracksystem(game->getgamestate().gettracksystems()),
    input(inp),
    orientor(game->getcamera())
{}

void Builder::render(Rendering* r)
{
    Vec mousepos = input.mapmousepos();
    orientor.endpoint(mousepos);
}

void Builder::leftclickmap(Vec mappos)
{
    orientor.startpoint(mappos);
}

void Builder::rightclickmap(Vec mappos)
{
    input.resetinput();
}

void Builder::leftreleasedmap(Vec mappos)
{
    if(orientor.anchordropped()){
        orientor.endpoint(mappos);
        if(canbuild()){
            build();
            game->getcontrolmode().account->
                pay(cost, Economy::PaymentType::buildings);
        }
        Builder::reset();
    }
}

void Builder::reset()
{
    orientor.reset();
}

bool Builder::canbuild()
{
    if(!game->getcontrolmode().account->canafford(cost))
        return false;
    if(!canfit())
        return false;
    return true;
}
