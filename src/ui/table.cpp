#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/input.h"
#include "bahnhof/common/timing.h"
#include "bahnhof/routing/routing.h"

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

TableLine::TableLine(Panel* newpanel, Table* newtable) : Element(newpanel)
{
    table = newtable;
    SDL_Rect tablerect = table->getlocalrect();
    rect = {tablerect.x, tablerect.y, 100, 20};
}

TableTextLine::TableTextLine(Panel* newpanel, Table* newtable, std::string newstr) : TableLine(newpanel, newtable)
{
    str = newstr;
}

void TableTextLine::render(Rendering* r, SDL_Rect maxarea)
{
    SDL_Rect textrect = r->rendertext(str, maxarea.x, maxarea.y, {0,0,0,0}, false, false, maxarea.w);
    rect.w = textrect.w;
    rect.h = textrect.h;
}

Table::Table(Panel* newpanel, SDL_Rect newrect) : Element(newpanel)
{
    rect = newrect;
}

void Table::render(Rendering* r)
{
    SDL_Rect maxarea = getglobalrect();
    for(auto& line: lines){
        line->render(r, maxarea);
        SDL_Rect textrect = line->getlocalrect();
        maxarea.y += textrect.h;
        maxarea.h -= textrect.h;
        if(maxarea.h<=0)
            return;
    }
}

MainInfoTable::MainInfoTable(Panel* newpanel, SDL_Rect newrect) : Table(newpanel, newrect) {}

void MainInfoTable::update(int ms)
{
    Gamestate* gamestate = &(ui->getgame().getgamestate());
    lines.clear();
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(int(gamestate->money)) + " Fr"));
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(int(gamestate->time*0.001/60)) + " min"));
    int income = int(60*float(gamestate->revenue)/float(gamestate->time*0.001/60));
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(income) + " Fr/h"));
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(game->gettimemanager().getfps()) + " fps"));
}

TrainTable::TrainTable(Panel* newpanel, SDL_Rect newrect) : Table(newpanel, newrect)
{
    trainmanager = &(ui->getgame().getgamestate().gettrainmanager());
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

void RoutesTable::update(int ms)
{
	RouteManager& routing = ui->getgame().getgamestate().getrouting();
	int scale = ui->getlogicalscale();
	int xoffset = 10*scale;
	int yoffset = (20+30)*scale;
	lines.clear();
	
	if(routing.selectedroute){
		std::vector<std::string> descriptions = routing.selectedroute->getorderdescriptions();
		std::vector<int> numbers = routing.selectedroute->getordernumberstorender();
        for(int iOrder = 0; iOrder<numbers.size(); iOrder++){
            std::string str = "("+std::to_string(numbers[iOrder])+") " + descriptions[iOrder];
            lines.emplace_back(new TableTextLine(panel, this, str));
        }
	}
	else{
		std::vector<std::string> names = routing.getroutenames();
        for(int iRoute = 0; iRoute<names.size(); iRoute++){
            std::string str = "(ctrl+"+std::to_string(iRoute+1)+") " + names[iRoute];
            lines.emplace_back(new TableTextLine(panel, this, str));
        }
        lines.emplace_back(new TableTextLine(panel, this, "(r) Create new route"));
    }
}

} //end namespace UI