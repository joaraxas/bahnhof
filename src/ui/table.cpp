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

bool Table::checkclick(Vec mousepos)
{
    bool clicked = Element::checkclick(mousepos);
	if(clicked){
        for(auto& line : lines){ //TODO: maybe expand a separate rect instead when adding lines
            if(line->checkclick(mousepos)){
                return true;
            }
        }
	}
    return false;
}

int Table::getlineindexat(Vec mousepos)
{
    int lineindex = -1;
    for(int index=0; index<lines.size(); index++)
        if(lines[index]->checkclick(mousepos)){
            lineindex = index;
            break;
        }
    return lineindex;
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

void Dropdown::render(Rendering* r)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    r->renderfilledrectangle(getglobalrect(), false, false);
    Table::render(r);
}

RouteDropdown::RouteDropdown(Panel* p, SDL_Rect r) : 
    Dropdown(p, r), 
    routing(ui->getgame().getgamestate().getrouting())
{}

void RouteDropdown::update(int ms)
{
	lines.clear();
	
    names = routing.getroutenames();
    ids = routing.getrouteids();
    for(int iRoute = 0; iRoute<names.size(); iRoute++){
        std::string name = names[iRoute];
        lines.emplace_back(new RouteTableLine(panel, this, name));
    }
}

void RouteDropdown::leftclick(Vec mousepos)
{
    int index = getlineindexat(mousepos);
    if(index>=0){
        dynamic_cast<TrainPanel*>(panel)->gettrain().route = routing.getroute(ids[index]);
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

TrainTable::TrainTable(Panel* newpanel, SDL_Rect newrect) : 
    Table(newpanel, newrect)
{
    trainmanager = &(ui->getgame().getgamestate().gettrainmanager());
}

void TrainTable::update(int ms)
{
	traininfos = trainmanager->gettrainsinfo();
	lines.clear();
	for(TrainInfo& info: traininfos){
        lines.emplace_back(new TrainTableLine(panel, this, info, trainmanager));
	}
}

void TrainTable::leftclick(Vec mousepos)
{
    int index = getlineindexat(mousepos);
    if(index>=0){
        trainmanager->deselectall();
        TrainInfo info = traininfos[index];
        info.train->selected = true;
        
        Vec viewsize = game->getrendering().getviewsize();
        int scale = ui->getlogicalscale();
        SDL_Rect trainpanelrect = {scale*300,scale*200,scale*400,scale*200};
        new TrainPanel(ui, trainpanelrect, *info.train);
    }
}

RouteTable::RouteTable(Panel* p, SDL_Rect r) : 
    Table(p, r), 
    routing(ui->getgame().getgamestate().getrouting())
{};

void RouteTable::update(int ms)
{
	lines.clear();
	
    names = routing.getroutenames();
    ids = routing.getrouteids();
    for(int iRoute = 0; iRoute<names.size(); iRoute++){
        std::string name = names[iRoute];
        lines.emplace_back(new RouteTableLine(panel, this, name));
    }
    lines.emplace_back(new RouteTableLine(panel, this, "New route"));
}

void RouteTable::leftclick(Vec mousepos)
{
    int index = getlineindexat(mousepos);
    if(index>=0){
        if(index<ids.size()){
            RouteListPanel* rlp = dynamic_cast<RouteListPanel*>(panel);
            rlp->addroutepanel(ids[index]);
        }
        else if(index==ids.size())
            routing.addroute();
    }
}

void OrderTable::update(int ms)
{
	lines.clear();
	
    descriptions = route->getorderdescriptions();
    orderids = route->getorderids();
    numbers = route->getordernumberstorender();
    for(int iOrder = 0; iOrder<numbers.size(); iOrder++){
        std::string str = "("+std::to_string(numbers[iOrder])+") " + descriptions[iOrder];
        int id = orderids[iOrder];
        lines.emplace_back(new OrderTableLine(panel, this, route, id, str));
    }
    if(lines.size() == 0)
        lines.emplace_back(new TableTextLine(panel, this, "No orders yet"));
}

void OrderTable::leftclick(Vec pos)
{
    int index = getlineindexat(pos);
    if(index>=0){
        route->selectedorderid = orderids[index];
    }
}

} //end namespace UI