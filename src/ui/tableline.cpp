#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/tables.h"
#include "bahnhof/ui/tablelines.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/rollingstock/train.h"
#include "bahnhof/buildings/buildingtypes.h"

namespace UI{

TableLine::TableLine(Host* p, Table* t, std::string s) : 
    Element(p),
    table(t),
    str(s)
{
    SDL_Rect tablerect = table->getlocalrect();
    rect = {tablerect.x, tablerect.y, tablerect.w, 20};
    SDL_Rect textrect = ui->getuirendering().gettextsize(str, rect, 1, 1);
    rect.h = textrect.h;
}

SDL_Rect TableLine::getglobalrect()
{
    SDL_Rect tablepos = table->getglobalrect();
    return {tablepos.x+rect.x, tablepos.y+rect.y, rect.w, rect.h};
}

void TableLine::render(Rendering* r, SDL_Rect maxarea, TextStyle style)
{
    rect.x = maxarea.x;
    rect.y = maxarea.y;
    ui->getuirendering().rendertext(r, str, getlocalrect(), style);
}

RouteTableLine::RouteTableLine(Host* p, Table* t, std::string routename) :
    TableLine(p, t, routename)
{}

void RouteTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    TableLine::render(r, maxarea);
    ui->getuirendering().renderrectangle(r, getlocalrect(), Info);
}

OrderTableLine::OrderTableLine(Host* p, Table* t, bool sel, std::string description) :
    selected(sel),
    TableLine(p, t, description)
{}

void OrderTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    TextStyle style;
    if(selected)
        style = Highlighted;
    else
        style = Info;
    TableLine::render(r, maxarea, style);
    ui->getuirendering().renderrectangle(r, getlocalrect(), style);
}

TrainTableLine::TrainTableLine(Host* p, Table* t, TrainInfo traininfo, TrainManager* manager) : 
    TableLine(p, t, traininfo.name), 
    info(traininfo),
    trainmanager(manager)
{}

void TrainTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    rect = maxarea;
    TextStyle style = Info;
    if(info.train->isselected())
        style = Highlighted;
    int rowoffset = 2;
    int textpadding = 5;
    int namerowwidth = 60;
    SDL_Rect namerect = getlocalrect();
    namerect.w = namerowwidth;
    
    namerect = ui->getuirendering().rendertext(r, str, namerect, style, false, textpadding, rowoffset);
    rect.h = namerect.h;
    
    SDL_Rect trainiconrect = getlocalrect();
    trainiconrect = {trainiconrect.x+namerowwidth+textpadding, 
                     trainiconrect.y+rowoffset, 
                     trainiconrect.w-namerowwidth-2*textpadding, 
                     namerect.h-2*rowoffset};
    // The local rectangle works here because we are rendering to a separate table target
    rendertrainicons(r, *ui, info.wagoninfos, trainiconrect);
    
    ui->getuirendering().renderrectangle(r, getlocalrect(), style);
}

PurchaseOptionTableLine::PurchaseOptionTableLine(Host* p, Table* t, sprites::name iconname, std::string name, float cost) : 
    TableLine(p, t, name),
    price(cost)
{
    SpriteManager& sprites = game->getsprites();
    icon.setspritesheet(sprites, iconname);
    icon.ported = false;
}

void PurchaseOptionTableLine::render(Rendering* r, SDL_Rect maxarea)
{
    rect = maxarea;
    TextStyle style = Info;
    Vec screeniconsize = icon.getsize();
    Vec uiiconsize = ui->getuirendering().screentoui(screeniconsize);
    // if(info.train->selected)
    //     style = Highlighted;
    int rowoffset = 2;
    int textpadding = 5;
    int pricerowwidth = 50;
    int iconwidth = uiiconsize.x + textpadding;
    int namerowwidth = getlocalrect().w - iconwidth - pricerowwidth;
    
    SDL_Rect namerect = getlocalrect();
    namerect.w = namerowwidth;
    namerect.x += iconwidth;
    namerect = ui->getuirendering().rendertext(r, str, namerect, style, false, textpadding, rowoffset);
    SDL_Rect pricerect = getlocalrect();
    pricerect.x = pricerect.x + pricerect.w - pricerowwidth;
    pricerect.w = pricerowwidth;
    pricerect = ui->getuirendering().rendertext(r, std::to_string(price)+" Fr", pricerect, style, false, textpadding, rowoffset);
    rect.h = std::fmax(std::fmax(namerect.h, pricerect.h), uiiconsize.y+2*rowoffset);
    SDL_Rect uiiconrect = getlocalrect();
    uiiconrect.x += textpadding;
    uiiconrect.y += rowoffset;
    SDL_Rect screeniconrect = ui->getuirendering().uitoscreen(uiiconrect);
    icon.render(r, Vec(screeniconrect.x+screeniconsize.x*0.5, screeniconrect.y+screeniconsize.y*0.5));
    
    ui->getuirendering().renderrectangle(r, getlocalrect(), style);
}

} //end namespace UI