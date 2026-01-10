#include "bahnhof/input/input.h"
#include "bahnhof/input/builder.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/camera.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/track/track.h"

IdleMode::IdleMode(Game& g) :
    trainmanager(g.getgamestate().gettrainmanager()),
    buildingmanager(g.getgamestate().getbuildingmanager()),
    tracksystem(g.getgamestate().gettracksystems())
{}

void IdleMode::leftclickmap(Vec mappos)
{
    if(buildingmanager.leftclick(mappos))
        return;
    
    Train* clickedtrain = trainmanager.gettrainatpos(mappos);
    if(clickedtrain){
        selecttrain(clickedtrain);
        return;
    }
    
    if(Tracks::Input::switchat(tracksystem, mappos)){
        return;
    }

    selecttrain(nullptr);
}

void IdleMode::selecttrain(Train* train)
{
    trainmanager.deselectall();
	if(train){
		train->select();
    }
}


DeleteMode::DeleteMode(Game& g) :
    buildingmanager(g.getgamestate().getbuildingmanager()),
    tracksystem(g.getgamestate().gettracksystems())
{}

void DeleteMode::leftclickmap(Vec mappos)
{
    Tracks::Input::deleteat(tracksystem, mappos);
}


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
    angleisset = false;
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
    angleisset = false;
    if(norm(diff) > 20/game->getcamera().getscale()){
        angle = Angle(diff);
        angleisset = true;
    }
}
