#include<format>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/tablelines.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/common/timing.h"
#include "bahnhof/input/input.h"
#include "bahnhof/buildings/buildingmanager.h"
#include "bahnhof/buildings/buildings.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/rollingstock/wagontypes.h"

namespace UI{

Table::Table(Host* newpanel, UIRect newrect) : 
    Element(newpanel), minsize(newrect.w, newrect.h)
{
    rect = newrect;
}

Table::~Table() // Needed to correctly destroy unique_ptrs
{}

bool Table::checkclick(UIVec mousepos)
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

void Table::render(Rendering* r)
{
    UIRect maxarea = {0,0,rect.w,rect.h};

    float scale = ui->getuirendering().getuiscale();
	SDL_Texture* tablerendertarget = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_TARGET, 
        ceil(maxarea.w*scale), 
        ceil(maxarea.h*scale)
    );
  	SDL_SetTextureBlendMode(tablerendertarget, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, tablerendertarget);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderFillRect(renderer, NULL);

    for(int index=0; index<lines.size(); index++){
        auto& line = lines[index];
        line->render(r, maxarea);
        UIRect textrect = line->getlocalrect();
        maxarea.y += textrect.h;
        maxarea.h -= textrect.h;
        if(maxarea.y>=rect.h){
            break;
        }
    }

    SDL_SetTextureAlphaMod(tablerendertarget, 255);
	SDL_SetRenderTarget(renderer, NULL);
    ui->getuirendering().rendertexture(r, tablerendertarget, getglobalrect());
    SDL_DestroyTexture(tablerendertarget);
}


UIVec Table::getminimumsize()
{
    return minsize + 2*getpadding();
}

UIRect Table::place(UIRect r)
{
    Element::place(r);
    rect.w = std::max(r.w-2*getpadding().x, minsize.x);
    rect.h = std::max(r.h-2*getpadding().y, minsize.y);
    return rect;
}

void ClickableTable::leftclick(UIVec pos)
{
    int nlines = lines.size();
    if(nlines > 0){
        int index = getlineindexat(pos);
        if(index>=0 && index<nlines){
            lineclicked(index);
        }
    }
}

void ClickableTable::scroll(UIVec pos, int distance)
{
    linescrolloffset -= 3*distance;

    Coord lineheightleft = 0;
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
            toplineindex = std::max(toplineindex-1, 0);
            linescrolloffset = linescrolloffset + lines[toplineindex]->getlocalrect().h;
        }
    }
}

int ClickableTable::getlineindexat(UIVec mousepos)
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

void ClickableTable::render(Rendering* r)
{
    UIRect maxarea = {0,-linescrolloffset,rect.w,rect.h};

    float scale = ui->getuirendering().getuiscale();
	SDL_Texture* tablerendertarget = SDL_CreateTexture(renderer, 
                                            SDL_PIXELFORMAT_RGBA8888, 
                                            SDL_TEXTUREACCESS_TARGET, 
                                            ceil(maxarea.w*scale), 
                                            ceil(maxarea.h*scale));
  	SDL_SetTextureBlendMode(tablerendertarget, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, tablerendertarget);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    SDL_RenderFillRect(renderer, NULL);

    for(int index=toplineindex; index<lines.size(); index++){
        auto& line = lines[index];
        TextStyle style = Info;
        if(index==selectedlineindex)
            style = Highlighted;
        line->render(r, maxarea, style, 2, 2);
        if(linescrolloffset>0 && index==toplineindex){
            /* small hack: render line as rectangle here to ensure same 
            integer rounding as for other rect renders */
            ui->getuirendering().renderrectangle(r, {0,0,rect.w,0}, style);
        }
        UIRect textrect = line->getlocalrect();
        maxarea.y += textrect.h;
        maxarea.h -= textrect.h;
        ui->getuirendering().renderrectangle(r, textrect, style);
        if(maxarea.y>=rect.h){
            // render line as rectangle here
            ui->getuirendering().renderrectangle(r, {0,rect.h,rect.w,0}, style);
            break;
        }
    }

    SDL_SetTextureAlphaMod(tablerendertarget, 255);
	SDL_SetRenderTarget(renderer, NULL);
    ui->getuirendering().rendertexture(r, tablerendertarget, getglobalrect());
    SDL_DestroyTexture(tablerendertarget);
}

Dropdown::Dropdown(Host* p, UIRect r) : ClickableTable(p, r)
{
    ui->setdropdown(this);
}

void Dropdown::render(Rendering* r)
{
    ClickableTable::render(r); // hack to get right table line heights
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    rect.h = 0;
    for(auto& line : lines){
        rect.h += line->getlocalrect().h;
    }

    ui->getuirendering().renderrectangle(r, getglobalrect(), InvertedInfo, true);
    ClickableTable::render(r);
}

RouteDropdown::RouteDropdown(Host* p, UIRect r) : 
    Dropdown(p, r), 
    routing(ui->getgame().getgamestate().getrouting())
{}

void RouteDropdown::update(int ms)
{
	lines.clear();
    names = routing.getroutenames();
    ids = routing.getrouteids();
    if(names.size()==0){
        lines.emplace_back(new TableLine(panel, this, "No routes created yet"));
    }
    else
        for(int iRoute = 0; iRoute<names.size(); iRoute++){
            std::string name = names[iRoute];
            lines.emplace_back(new TableLine(panel, this, name));
        }
}

void RouteDropdown::lineclicked(int index)
{
    names = routing.getroutenames();
    ids = routing.getrouteids();
    if(index<ids.size())
        dynamic_cast<TrainPanel*>(panel)->gettrain().route = routing.getroute(ids[index]);
}

MainInfoTable::MainInfoTable(Host* newpanel, UIRect newrect) : Table(newpanel, newrect) {}

void MainInfoTable::update(int ms)
{
    Gamestate& gamestate = ui->getgame().getgamestate();
    InputManager& input = ui->getgame().getinputmanager();
    lines.clear();
    lines.emplace_back(new TableLine(panel, this, std::to_string(int(gamestate.money)) + " Fr"));
    lines.emplace_back(new TableLine(panel, this, std::to_string(int(gamestate.time*0.001/60)) + " min"));
    int income = int(60*float(gamestate.revenue)/float(gamestate.time*0.001/60));
    lines.emplace_back(new TableLine(panel, this, std::to_string(income) + " Fr/h"));
    lines.emplace_back(new TableLine(panel, this, std::to_string(game->gettimemanager().getfps()) + " fps"));
    lines.emplace_back(new TableLine(panel, this, std::to_string(int(input.mapmousepos().x))+","+std::to_string(int(input.mapmousepos().y))));
}


TrainTable::TrainTable(Host* newpanel, UIRect newrect) : 
    ClickableTable(newpanel, newrect)
{
    trainmanager = &(ui->getgame().getgamestate().gettrainmanager());
}

void TrainTable::update(int ms)
{
	traininfos = trainmanager->gettrainsinfo();
	lines.clear();
    selectedlineindex = -1;
    for(int index=0; index<traininfos.size(); index++){
	    TrainInfo& info = traininfos[index];
        lines.emplace_back(new TrainTableLine(panel, this, info));
        if(info.train->isselected())
            selectedlineindex = index;
	}
}

void TrainTable::lineclicked(int index)
{
    traininfos = trainmanager->gettrainsinfo();
    trainmanager->deselectall();
    TrainInfo info = traininfos[index];
    info.train->select();
}


void TrainInfoTable::update(int ms)
{
	TrainInfo info = train.getinfo();
    lines.clear();
    float mps = abs(info.speed*0.001*150);
    float kmh = mps*3.6;
    lines.emplace_back(new TableLine(panel, this, std::format("{0:.1f} km/h", kmh)));
    // lines.emplace_back(new TableLine(panel, this, std::format("{0:.1f} m/s", abs(mps))));
}


RouteTable::RouteTable(Host* p, UIRect r) : 
    ClickableTable(p, r), 
    routing(ui->getgame().getgamestate().getrouting())
{};

void RouteTable::update(int ms)
{
	lines.clear();
	
    names = routing.getroutenames();
    ids = routing.getrouteids();
    for(int iRoute = 0; iRoute<names.size(); iRoute++){
        std::string name = names[iRoute];
        lines.emplace_back(new TableLine(panel, this, name));
    }
    lines.emplace_back(new TableLine(panel, this, "New route"));
}

void RouteTable::lineclicked(int index)
{
    if(index<ids.size()){
        RouteListPanel* rlp = dynamic_cast<RouteListPanel*>(panel);
        rlp->addroutepanel(ids[index]);
    }
    else if(index==ids.size())
        routing.addroute();
}


void OrderTable::update(int ms)
{
	lines.clear();
    selectedlineindex = -1;
	
    if(route){
        descriptions = route->getorderdescriptions();
        orderids = route->getorderids();
        numbers = route->getordernumberstorender();
        for(int iOrder = 0; iOrder<numbers.size(); iOrder++){
            std::string str = "("+std::to_string(numbers[iOrder])+") " + descriptions[iOrder];
            int id = orderids[iOrder];
            if(id==route->selectedorderid)
                selectedlineindex = iOrder;
            lines.emplace_back(new TableLine(panel, this, str));
        }
    }
    if(lines.size() == 0)
        lines.emplace_back(new TableLine(panel, this, "No orders yet"));
}

void OrderTable::render(Rendering* r)
{
    if(orderids.empty())
        Table::render(r);
    else   
        ClickableTable::render(r);
}

void OrderTable::lineclicked(int index)
{
    if(index<orderids.size())
        route->selectedorderid = orderids[index];
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

void TrainOrderTable::lineclicked(int index)
{
    if(index<orderids.size())
        train.orderid = orderids[index];
}


ConstructionTable::ConstructionTable(Host* p, UIRect r) : 
    ClickableTable(p, r), 
    input(game->getinputmanager()),
    buildingtypes(game->getgamestate().getbuildingmanager().gettypes())
{
    for(int i=0; i<buildingtypes.size(); i++){
        const BuildingType& type = buildingtypes[i];
        lines.emplace_back(
            new PurchaseOptionTableLine(panel, 
                this,
                type.iconname,
                type.name,
                type.cost
            )
        );
    }
}

void ConstructionTable::lineclicked(int index)
{
    BuildingManager& buildings = game->getgamestate().getbuildingmanager();
    const BuildingType& clickedbuilding = buildingtypes.at(index); // TODO: Highlight the one currently being built
    input.selectbuildingtoplace(&clickedbuilding);
}


WagonTable::WagonTable(Host* p, WagonFactory& f, UIRect r) : 
    ClickableTable(p, r), 
    input(game->getinputmanager()),
    factory(f)
{
    for(WagonType* type : factory.getavailabletypes()){
        lines.emplace_back(
            new PurchaseOptionTableLine(panel, 
                this,
                type->iconname,
                type->name,
                type->cost
            )
        );
    }
}

void WagonTable::lineclicked(int index)
{
    const WagonType* clickedwagon = factory.getavailabletypes().at(index);
    factory.orderwagon(*clickedwagon);
}

} //end namespace UI