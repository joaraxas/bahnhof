#include<iostream>
#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"


UIRendering::UIRendering(InterfaceManager& newui) : ui(newui)
{
    setuiscale(ui.getgame().getrendering().getlogicalscale());
}

void UIRendering::rendertexture(Rendering* r, SDL_Texture* tex, SDL_Rect* rect, SDL_Rect* srcrect, float angle, bool ported, bool originiscenter, int centerx, int centery)
{
    SDL_Rect screenrect = uitoscreen(*rect);
    r->rendertexture(tex, &screenrect, srcrect, angle, ported, false, originiscenter, centerx, centery);
}

SDL_Color UIRendering::getcolorfromstyle(UI::TextStyle style)
{
    SDL_Color color;
    switch (style)
    {
    case UI::Info:
        color = {0,0,0,255};
        break;
    case UI::Highlighted:
        color = {255,255,255,255};
        break;
    case UI::InvertedInfo:
        color = {255,255,255,255};
        break;
    case UI::MapOverlay:
        color = {255,255,255,255};
        break;
    default:
        std::cout<<"Warning: UI style "<<style<<" not supported in InterfaceManager::rendertext"<<std::endl;
        break;
    }
    return color;
}

SDL_Rect UIRendering::rendertext(Rendering* r, std::string text, SDL_Rect rect, UI::TextStyle style, bool centered, int margin_x, int margin_y)
{
    SDL_Color color = getcolorfromstyle(style);
    rect.x += margin_x;
    rect.w -= 2*margin_x;
    rect.y += margin_y;
    rect.h -= 2*margin_y;
    SDL_Rect absrect = uitoscreen(rect);
    SDL_Rect screentextrect;
    if(centered)
        screentextrect = r->rendercenteredtext(text, absrect.x+absrect.w*0.5, absrect.y+absrect.h*0.5, color, false, false, absrect.w);
    else
        screentextrect = r->rendertext(text, absrect.x, absrect.y, color, false, false, absrect.w);
    SDL_Rect textrect = screentoui(screentextrect);
    textrect.x -= margin_x;
    textrect.w += 2*margin_x;
    textrect.y -= margin_y;
    textrect.h += 2*margin_y;
    return textrect;
}

SDL_Rect UIRendering::gettextsize(std::string text, SDL_Rect maxrect, int margin_x, int margin_y)
{
    int maxtextuiwidth = maxrect.w - 2*margin_x;
    SDL_Texture* tex =  loadtext(text, {0,0,0,255}, maxtextuiwidth*uiscale);
    int textscreenwidth, textscreenheight;
	SDL_QueryTexture(tex, NULL, NULL, &textscreenwidth, &textscreenheight);
	SDL_DestroyTexture(tex);
    int textuiwidth = textscreenwidth/uiscale;
    int textuiheight = textscreenheight/uiscale;
    return SDL_Rect({maxrect.x, maxrect.y, textuiwidth+2*margin_x, textuiheight+2*margin_y});
}

float UIRendering::getuiscale()
{
    return uiscale;
}

void UIRendering::increaseuiscale()
{
    setuiscale(uiscale + 0.2);
}

void UIRendering::decreaseuiscale()
{
    setuiscale(uiscale - 0.2);
}

void UIRendering::setuiscale(float newscale)
{
    uiscale = std::fmax(0.8, newscale);;
    int newfontsize = 12*uiscale;
    setfontsize(newfontsize);
}

SDL_Rect UIRendering::uitoscreen(SDL_Rect uirect)
{
    float scale = getuiscale();
    SDL_Rect screenrect = {int(round(uirect.x*scale)), int(round(uirect.y*scale)), int(round(uirect.w*scale)), int(round(uirect.h*scale))};
    return screenrect;
}

SDL_Rect UIRendering::screentoui(SDL_Rect screenrect)
{
    float scale = getuiscale();
    SDL_Rect uirect = {int(round(screenrect.x/scale)), int(round(screenrect.y/scale)), int(round(screenrect.w/scale)), int(round(screenrect.h/scale))};
    return uirect;
}

Vec UIRendering::uitoscreen(Vec pos)
{
    float scale = getuiscale();
    Vec uipos = Vec(pos.x*scale, pos.y*scale);
    return uipos;
}

Vec UIRendering::screentoui(Vec pos)
{
    float scale = getuiscale();
    Vec uipos = Vec(pos.x/scale, pos.y/scale);
    return uipos;
}

void UIRendering::renderscaleruler(Rendering* r, int leftx, int lefty, int scalelinelength)
{
    float uiscale = getuiscale();

    int textheight = 14;
    SDL_Color c = getcolorfromstyle(UI::MapOverlay);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    int markersize = 2;
	r->renderline(uitoscreen(Vec(leftx,lefty)), uitoscreen(Vec(leftx+scalelinelength,lefty)), false);
	r->renderline(uitoscreen(Vec(leftx,lefty-markersize)), uitoscreen(Vec(leftx,lefty+markersize)), false);
	r->renderline(uitoscreen(Vec(leftx+scalelinelength,lefty-markersize)), uitoscreen(Vec(leftx+scalelinelength,lefty+markersize)), false);
    std::string scaletext = std::to_string(int(round(scalelinelength*0.001*150/r->getcamscale()*uiscale))) + " m";
	rendertext(r, scaletext, {leftx, lefty-textheight, scalelinelength, textheight}, UI::MapOverlay, true);
}