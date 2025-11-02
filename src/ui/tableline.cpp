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
}

UIRect TableLine::getglobalrect()
{
    UIRect tablepos = table->getglobalrect();
    return {tablepos.x+rect.x, tablepos.y+rect.y, rect.w, rect.h};
}

void TableLine::render(Rendering* r, SDL_Rect maxarea, TextStyle style, int xmargin, int ymargin)
{
    rect.x = maxarea.x;
    rect.y = maxarea.y;
    SDL_Rect textrect = ui->getuirendering().rendertext(r, str, getlocalrect(), style, false, xmargin, ymargin);
    rect.h = textrect.h;
}

TrainTableLine::TrainTableLine(Host* p, Table* t, TrainInfo traininfo) : 
    TableLine(p, t, traininfo.name), 
    info(traininfo)
{}

void TrainTableLine::render(
    Rendering* r, 
    SDL_Rect maxarea, 
    TextStyle style, 
    int xmargin, 
    int ymargin
    )
{
    rect = maxarea;
    int textpadding = 5;
    int namerowwidth = 60;
    SDL_Rect namerect = getlocalrect();
    namerect.w = namerowwidth;
    namerect.x = xmargin;
    
    namerect = ui->getuirendering().rendertext(
        r, str, namerect, style, false, textpadding, ymargin);
    rect.h = namerect.h;
    
    SDL_Rect trainiconrect = getlocalrect();
    trainiconrect = {trainiconrect.x+xmargin+namerowwidth, 
                     trainiconrect.y+ymargin, 
                     trainiconrect.w-namerowwidth-2*xmargin, 
                     namerect.h-2*ymargin};
    // The local rectangle works here because we are rendering to a 
    // separate table target
    rendertrainicons(r, *ui, info.wagoninfos, trainiconrect);
}

PurchaseOptionTableLine::PurchaseOptionTableLine(Host* p, Table* t, sprites::name iconname, std::string name, float cost) : 
    TableLine(p, t, name),
    price(cost)
{
    SpriteManager& sprites = game->getsprites();
    icon.setspritesheet(sprites, iconname);
    icon.ported = false;
}

void PurchaseOptionTableLine::render(Rendering* r, SDL_Rect maxarea, TextStyle style, int xmargin, int ymargin)
{
    rect = maxarea;
    UIRendering& uiren = ui->getuirendering();
    Vec screeniconsize = icon.getsize();
    Vec uiiconsize = uiren.screentoui(screeniconsize);
    int rowoffset = ymargin;
    int textpadding = 5;
    int pricerowwidth = 50;
    int iconwidth = uiiconsize.x + xmargin + textpadding;
    int namerowwidth = getlocalrect().w - iconwidth - pricerowwidth;
    
    SDL_Rect namerect = getlocalrect();
    namerect.w = namerowwidth;
    namerect.x += iconwidth;
    namerect.y += uiiconsize.y * 0.5 - 12 * 0.5 - 1;
    namerect = uiren.rendertext(
        r, str, namerect, style, false, textpadding, rowoffset);
    SDL_Rect pricerect = getlocalrect();
    pricerect.x = pricerect.x + pricerect.w - pricerowwidth;
    pricerect.w = pricerowwidth;
    pricerect.y = namerect.y;
    pricerect = uiren.rendertext(
                    r, 
                    std::to_string(price)+" Fr", 
                    pricerect, 
                    style, 
                    false, 
                    textpadding, 
                    rowoffset);
    rect.h = std::fmax(
        std::fmax(namerect.h, pricerect.h), 
        uiiconsize.y+2*rowoffset);
    SDL_Rect uiiconrect = getlocalrect();
    uiiconrect.x += xmargin + textpadding;
    uiiconrect.y += rowoffset;
    SDL_Rect screeniconrect = uiren.uitoscreen(uiiconrect);
    icon.render(r, 
                Vec(screeniconrect.x+screeniconsize.x*0.5, 
                screeniconrect.y+screeniconsize.y*0.5)
                );
}

} //end namespace UI