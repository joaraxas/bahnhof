#include<iostream>
#include<format>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/timing.h"
#include "bahnhof/common/input.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"

namespace UI{

Table::Table(Host* newpanel, SDL_Rect newrect) : Element(newpanel)
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

void Table::scroll(Vec pos, int distance)
{
    linescrolloffset -= 3*distance;

    int lineheightleft = 0;
    for(int index=toplineindex; index<lines.size(); index++){
        lineheightleft += lines[index]->getlocalrect().h;
        if(lineheightleft>rect.h+linescrolloffset){
            lineheightleft = -1;
            break;
        }
    }
    if(lineheightleft != -1){
        linescrolloffset = lineheightleft - rect.h;
    }

    while(linescrolloffset > lines[toplineindex]->getlocalrect().h){
        linescrolloffset = linescrolloffset - lines[toplineindex]->getlocalrect().h;
        toplineindex = std::fmin(toplineindex+1, lines.size()-1);
    }
    while(linescrolloffset < 0){
        if(toplineindex == 0)
            linescrolloffset = 0;
        else{
            toplineindex = std::fmax(toplineindex-1, 0);
            linescrolloffset = linescrolloffset + lines[toplineindex]->getlocalrect().h;
        }
    }
}

int Table::getlineindexat(Vec mousepos)
{
    int lineindex = -1;
    int numlines = lines.size();
    for(int index=toplineindex; index<numlines; index++)
        if(lines[index]->checkclick(mousepos)){
            lineindex = index;
            break;
        }
    return lineindex;
}

void Table::render(Rendering* r)
{
    SDL_Rect maxarea = {0,-linescrolloffset,rect.w,rect.h};

    float scale = ui->getuirendering().getuiscale();
	SDL_Texture* result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ceil(maxarea.w*scale), ceil(maxarea.h*scale));
  	SDL_SetTextureBlendMode(result, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, result);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderFillRect(renderer, NULL);

    for(int index=toplineindex; index<lines.size(); index++){
        auto& line = lines[index];
        line->render(r, maxarea);
        if(linescrolloffset>0 && index==toplineindex){
            /* small hack: render line as rectangle here to use same color as in line->render 
               and ensure same integer rounding as for other rect renders */
            r->renderrectangle(ui->getuirendering().uitoscreen({0,0,rect.w,0}), false, false);
        }
        SDL_Rect textrect = line->getlocalrect();
        maxarea.y += textrect.h;
        maxarea.h -= textrect.h;
        if(maxarea.y>=rect.h){
            // render line as rectangle here to use same color as in line->render
            r->renderrectangle(ui->getuirendering().uitoscreen({0,rect.h,rect.w,0}), false, false);
            break;
        }
    }

    SDL_SetTextureAlphaMod(result, 255);
	SDL_SetRenderTarget(renderer, NULL);
    SDL_Rect global = getglobalrect();
    ui->getuirendering().rendertexture(r, result, &global);
    SDL_DestroyTexture(result);
}

Dropdown::Dropdown(Host* p, SDL_Rect r) : Table(p, r)
{
    ui->setdropdown(this);
}

void Dropdown::render(Rendering* r)
{
    Table::render(r); // hack to get right table line heights
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    rect.h = 0;
    for(auto& line : lines){
        rect.h += line->getlocalrect().h;
    }

    r->renderfilledrectangle(ui->getuirendering().uitoscreen(getglobalrect()), false, false);
    Table::render(r);
}

RouteDropdown::RouteDropdown(Host* p, SDL_Rect r) : 
    Dropdown(p, r), 
    routing(ui->getgame().getgamestate().getrouting())
{}

void RouteDropdown::update(int ms)
{
	lines.clear();
    names = routing.getroutenames();
    ids = routing.getrouteids();
    if(names.size()==0){
        lines.emplace_back(new TableTextLine(panel, this, "No routes created yet"));
    }
    else
        for(int iRoute = 0; iRoute<names.size(); iRoute++){
            std::string name = names[iRoute];
            lines.emplace_back(new RouteTableLine(panel, this, name));
        }
}

void RouteDropdown::leftclick(Vec mousepos)
{
    if(routing.getnumberofroutes() > 0){
        int index = getlineindexat(mousepos);
        if(index>=0){
            dynamic_cast<TrainPanel*>(panel)->gettrain().route = routing.getroute(ids[index]);
        }
    }
}

MainInfoTable::MainInfoTable(Host* newpanel, SDL_Rect newrect) : Table(newpanel, newrect) {}

void MainInfoTable::update(int ms)
{
    Gamestate& gamestate = ui->getgame().getgamestate();
    InputManager& input = ui->getgame().getinputmanager();
    lines.clear();
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(int(gamestate.money)) + " Fr"));
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(int(gamestate.time*0.001/60)) + " min"));
    int income = int(60*float(gamestate.revenue)/float(gamestate.time*0.001/60));
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(income) + " Fr/h"));
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(game->gettimemanager().getfps()) + " fps"));
    lines.emplace_back(new TableTextLine(panel, this, std::to_string(int(input.mapmousepos().x))+","+std::to_string(int(input.mapmousepos().y))));
}

TrainTable::TrainTable(Host* newpanel, SDL_Rect newrect) : 
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
        
        SDL_Rect trainpanelrect = {300,200,400,220};
        new TrainPanel(ui, trainpanelrect, *trainmanager, *info.train);
    }
}

void TrainInfoTable::update(int ms)
{
	TrainInfo info = train.getinfo();
    lines.clear();
    lines.emplace_back(new TableTextLine(panel, this, info.name));
    float mps = abs(info.speed*0.001*150);
    float kmh = mps*3.6;
    lines.emplace_back(new TableTextLine(panel, this, std::format("{0:.1f} km/h", kmh)));
    // lines.emplace_back(new TableTextLine(panel, this, std::format("{0:.1f} m/s", abs(mps))));
}

RouteTable::RouteTable(Host* p, SDL_Rect r) : 
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
	
    if(route){
        descriptions = route->getorderdescriptions();
        orderids = route->getorderids();
        numbers = route->getordernumberstorender();
        for(int iOrder = 0; iOrder<numbers.size(); iOrder++){
            std::string str = "("+std::to_string(numbers[iOrder])+") " + descriptions[iOrder];
            int id = orderids[iOrder];
            bool isselected = (id==route->selectedorderid);
            lines.emplace_back(new OrderTableLine(panel, this, isselected, id, str));
        }
    }
    if(lines.size() == 0)
        lines.emplace_back(new TableTextLine(panel, this, "No orders yet"));
}

void OrderTable::leftclick(Vec pos)
{
    int index = getlineindexat(pos);
    if(index>=0 && route && index<orderids.size()){
        route->selectedorderid = orderids[index];
    }
}

void TrainOrderTable::update(int ms)
{
    route = train.route;
    int id;
    if(route){
        id = route->selectedorderid;
        route->selectedorderid = train.orderid; // TODO: This is a temporary hack, should extend this functionality to abstract table class
    }
    OrderTable::update(ms);
    if(route)
        route->selectedorderid = id;
}

void TrainOrderTable::leftclick(Vec pos)
{
    int index = getlineindexat(pos);
    if(index>=0 && route && index<orderids.size()){
        train.orderid = orderids[index];
    }
}

} //end namespace UI