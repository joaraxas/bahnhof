#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/timing.h"
#include "bahnhof/routing/routing.h"

namespace UI{

Table::Table(Panel* newpanel, SDL_Rect newrect) : Element(newpanel)
{
    rect = newrect;
}

bool Table::checkclick(Vec mousepos, int type)
{
    bool clicked = Element::checkclick(mousepos, type);
	if(clicked){
        for(auto& line : lines){
            if(line->checkclick(mousepos, type)){
                return true;
            }
        }
	}
    return false;
}

void Table::render(Rendering* r)
{
    SDL_Rect maxarea = {0,0,rect.w,rect.h};
    for(auto& line: lines){
        line->render(r, maxarea);
        SDL_Rect textrect = line->getlocalrect();
        maxarea.y += textrect.h;
        maxarea.h -= textrect.h;
        if(maxarea.h<=0)
            return;
    }
}

Dropdown::Dropdown(Panel* p, SDL_Rect r) : Table(p, r)
{
    ui->setdropdown(this);
}

void RouteDropdown::update(int ms)
{
	lines.clear();
	
	RouteManager& routing = ui->getgame().getgamestate().getrouting();
    std::vector<std::string> names = routing.getroutenames();
    std::vector<int> ids = routing.getrouteids();
    for(int iRoute = 0; iRoute<names.size(); iRoute++){
        int id = ids[iRoute];
        std::string name = names[iRoute];
        std::string str = name;
        lines.emplace_back(new SelectRouteTableLine(panel, this, str, id));
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

TrainListTable::TrainListTable(Panel* newpanel, SDL_Rect newrect) : Table(newpanel, newrect)
{
    trainmanager = &(ui->getgame().getgamestate().gettrainmanager());
}

void TrainListTable::update(int ms)
{
	std::vector<TrainInfo> traininfos = trainmanager->gettrainsinfo();
	lines.clear();
	for(TrainInfo& info: traininfos){
        lines.emplace_back(new TrainTableLine(panel, this, info, trainmanager));
	}
}

void RouteListTable::update(int ms)
{
	lines.clear();
	
	RouteManager& routing = ui->getgame().getgamestate().getrouting();
    std::vector<std::string> names = routing.getroutenames();
    std::vector<int> ids = routing.getrouteids();
    for(int iRoute = 0; iRoute<names.size(); iRoute++){
        int id = ids[iRoute];
        std::string name = names[iRoute];
        std::string str = name;
        lines.emplace_back(new RouteTableLine(panel, this, str, id));
    }
    lines.emplace_back(new NewRouteTableLine(panel, this));
}

void RouteTable::update(int ms)
{
	lines.clear();
	
    std::vector<std::string> descriptions = route->getorderdescriptions();
    std::vector<int> ids = route->getorderids();
    std::vector<int> numbers = route->getordernumberstorender();
    for(int iOrder = 0; iOrder<numbers.size(); iOrder++){
        std::string str = "("+std::to_string(numbers[iOrder])+") " + descriptions[iOrder];
        int id = ids[iOrder];
        lines.emplace_back(new OrderTableLine(panel, this, route, id, str));
    }
    if(lines.size() == 0)
        lines.emplace_back(new TableTextLine(panel, this, "No orders yet"));
}

} //end namespace UI