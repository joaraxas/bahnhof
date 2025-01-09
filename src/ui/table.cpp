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

TableLine::TableLine(Panel* newpanel, Table* newtable) : Element(newpanel)
{
    table = newtable;
    SDL_Rect tablerect = table->getlocalrect();
    rect = {tablerect.x, tablerect.y, 100, 20};
}

SDL_Rect TableLine::getglobalrect()
{
    SDL_Rect tablepos = table->getglobalrect();
    return {rect.x+int(tablepos.x), rect.y+int(tablepos.y), rect.w, rect.h};
}

TableTextLine::TableTextLine(Panel* newpanel, Table* newtable, std::string newstr) : Element(newpanel), TableLine(newpanel, newtable)
{
    str = newstr;
}

void TableTextLine::render(Rendering* r, SDL_Rect maxarea)
{
    rect = maxarea;
    SDL_Rect globrect = getglobalrect();
    SDL_Rect textrect = r->rendertext(str, globrect.x, globrect.y, {0,0,0,0}, false, false, globrect.w);
    rect.h = textrect.h;
}

RouteTableLine::RouteTableLine(Panel* p, Table* t, std::string routename, int i) :
    Element(p), 
    routeindex(i),
    TableTextLine(p, t, routename), 
    Button(p, Vec(0,0))
{}

void RouteTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    TableTextLine::render(r, maxarea);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    r->renderrectangle(getglobalrect(), false, false);
}

void RouteTableLine::click()
{
    game->getgamestate().getrouting().selectroute(routeindex);
}

NewRouteTableLine::NewRouteTableLine(Panel* p, Table* t) :
    Element(p),
    RouteTableLine(p, t, "New route", 0)
{}

void NewRouteTableLine::click()
{
    game->getgamestate().getrouting().addroute();
}

TrainTableLine::TrainTableLine(Panel* p, Table* t, TrainInfo traininfo, TrainManager* manager) : 
    Element(p), 
    TableLine(p, t), 
    Button(p, Vec(0,0)), 
    info(traininfo),
    trainmanager(manager)
{}

void TrainTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    rect = maxarea;
    auto scale = ui->getlogicalscale();
    Uint8 intensity = 255*info.train->selected;
    SDL_Rect absrect = getglobalrect();
    int rowoffset = 3*scale;
    int textpadding = 1*scale;
    int namerowwidth = 40*scale;
    SDL_SetRenderDrawColor(renderer,intensity,intensity,intensity,255);
    
    SDL_Rect namerect = r->rendertext(info.name, absrect.x+rowoffset, absrect.y+textpadding, {intensity, intensity, intensity, 255}, false, false, namerowwidth-rowoffset);
    absrect.h = namerect.h + 2*textpadding;
    
    int iconoffset = 2*scale;
	SpriteManager& spritemanager = ui->getgame().getsprites();
    Sprite wagonicon;
    wagonicon.ported = false;
    wagonicon.zoomed = false;
    int icon_x = namerowwidth + rowoffset;
    for(WagonInfo& wagoninfo : info.wagoninfos){
        wagonicon.setspritesheet(spritemanager, wagoninfo.iconname);
        Vec iconsize = wagonicon.getsize();
        wagonicon.render(r, Vec(absrect.x+icon_x+iconsize.x*0.5, absrect.y+textpadding+namerect.h*0.5));
        icon_x += iconsize.x + iconoffset;
    }
    
    rect.h = absrect.h;
    r->renderrectangle(getglobalrect(), false, false);
}

void TrainTableLine::click()
{
    trainmanager->deselectall();
    info.train->selected = true;
}


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
	std::vector<TrainInfo> traininfos = trainmanager->gettrainsinfo();
	lines.clear();
	for(TrainInfo& info: traininfos){
        lines.emplace_back(new TrainTableLine(panel, this, info, trainmanager));
	}
}

void RoutesTable::update(int ms)
{
	RouteManager& routing = ui->getgame().getgamestate().getrouting();
	lines.clear();
	
	if(routing.selectedroute){
		std::vector<std::string> descriptions = routing.selectedroute->getorderdescriptions();
		std::vector<int> numbers = routing.selectedroute->getordernumberstorender();
        for(int iOrder = 0; iOrder<numbers.size(); iOrder++){
            std::string str = "("+std::to_string(numbers[iOrder])+") " + descriptions[iOrder];
            lines.emplace_back(new RouteTableLine(panel, this, str, iOrder));
        }
	}
	else{
		std::vector<std::string> names = routing.getroutenames();
		std::vector<int> ids = routing.getrouteids();
        for(int iRoute = 0; iRoute<names.size(); iRoute++){
            int id = ids[iRoute];
            std::string name = names[iRoute];
            std::string str = "(ctrl+"+std::to_string(id)+") " + name;
            lines.emplace_back(new RouteTableLine(panel, this, str, id));
        }
        lines.emplace_back(new NewRouteTableLine(panel, this));
    }
}

} //end namespace UI