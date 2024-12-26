#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"

namespace UI{

Element::Element(Panel* newpanel)
{
    panel = newpanel;
    ui = &panel->getui();
    game = &ui->getgame();
}

SDL_Rect Element::getglobalrect()
{
    Vec panelpos = panel->topcorner();
    return {rect.x+int(panelpos.x), rect.y+int(panelpos.y), rect.w, rect.h};
}

SDL_Rect Element::getlocalrect()
{
    return rect;
}

TrainTable::TrainTable(Panel* newpanel, SDL_Rect newrect) : Element(newpanel)
{
    trainmanager = &(ui->getgame().getgamestate().gettrainmanager());
    rect = newrect;
}

void TrainTable::update(int ms)
{
	traininfos = trainmanager->gettrainsinfo();
    trainstrings.clear();
	for(TrainInfo& info: traininfos){
		trainstrings.push_back(info.name);
		trainspeeds.push_back(info.speed);
	}
}

void TrainTable::render(Rendering* r)
{
	// Train* selectedtrain = game->getinputmanager().getselectedtrain();
	int scale = ui->getlogicalscale();
	int tableheight = r->rendertable(trainstrings, getglobalrect());
}

} //end namespace UI