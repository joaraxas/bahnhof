#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/input/input.h"
#include "bahnhof/input/textinput.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/decoration.h"
#include "bahnhof/rollingstock/trainmanager.h"
#include "bahnhof/rollingstock/train.h"

namespace UI{

Text::Text(Host* p, std::string t, UIRect r) : Element(p), text(t)
{
    rect = r;
}

void Text::render(Rendering* r)
{
    ui->getuirendering().rendertext(
        r, text, getglobalrect(), style, centered, margin_x, margin_y);
}

EditableText::EditableText(Host* p, std::string& t, UIRect r) : 
        Text(p, t, r), 
        textreference(t), 
        originalrect(r),
        shortenedtext(t) {
    shortenedtext = ui->getuirendering().croptexttowidth(
        text, rect.w, margin_x);
};

EditableText::~EditableText()
{
    if(beingedited){
        game->getinputmanager().gettextinputmanager().endtextinput();
    }
}

void EditableText::leftclick(UIVec mousepos)
{
    ui->getgame().getinputmanager().gettextinputmanager().starttextinput(this);
}

void EditableText::render(Rendering* r)
{
    if(beingedited){
        ui->getuirendering().renderrectangle(
            r, getglobalrect(), InvertedInfo, true);
        std::string textwithcursor = text;
        textwithcursor.insert(textwithcursor.begin()+cursorindex, '|');
        ui->getuirendering().rendertext(
            r, textwithcursor, getglobalrect(), Info, 
            centered, margin_x, margin_y);
    }
    else{
        ui->getuirendering().rendertext(
            r, shortenedtext, getglobalrect(), style, centered, 
            margin_x, margin_y);
    }
    ui->getuirendering().renderrectangle(r, getglobalrect(), InvertedInfo);
}

void EditableText::updatesource()
{
    if(!text.empty()){
        textreference = text;
        fallbacktext = text;
    }
}

void EditableText::startwriting(){
    cursorindex = text.size();
    beingedited = true;
    fallbacktext = text;
    updatewritingarea();
}

void EditableText::stopwriting(){
    text = fallbacktext;
    beingedited = false;
    rect.w = originalrect.w;
    rect.h = originalrect.h;
    shortenedtext = ui->getuirendering().croptexttowidth(
        text, rect.w, margin_x);
}

void EditableText::deleteselection(){
    if(!text.empty() && cursorindex>0){
        text.erase(cursorindex-1, 1);
        cursorindex--;
        updatewritingarea();
    }
}

void EditableText::addtext(const std::string& string){
    text.insert(cursorindex, string);
    cursorindex += string.size();
    updatewritingarea();
}

void EditableText::movecursorleft(){
    cursorindex = std::max(0, cursorindex-1);
}

void EditableText::movecursorright(){
    cursorindex = std::fmin(text.size(), cursorindex+1);
}

void EditableText::updatewritingarea(){
    UIRect textrect = ui->getuirendering().gettextsize(
        text+"|", originalrect, margin_x, margin_y);
    rect.h = std::max(textrect.h, originalrect.h);
}


void TrainIcons::render(Rendering* r)
{
    TrainInfo traininfo = train.getinfo();
    iconrects = rendertrainicons(
        r, *ui, traininfo.wagoninfos, getglobalrect(), 
        rendersplitafterwagonid);
    rendersplitafterwagonid = -1;
}

void TrainIcons::mousehover(UIVec pos, int ms)
{
    rendersplitafterwagonid = std::fmin(
        getwagonidatmousepos(pos), 
        train.getinfo().wagoninfos.size()-2
    );
}

void TrainIcons::leftclick(UIVec mousepos)
{
    int wagonid = getwagonidatmousepos(mousepos);
    if(wagonid<0) return;
    int wheretosplit = 0;
    if(!train.gasisforward){
        wheretosplit = iconrects.size()-wagonid-1;
    }
    else{
        wheretosplit = wagonid + 1;
    }
    train.split(wheretosplit, nullptr);
}

int TrainIcons::getwagonidatmousepos(UIVec mousepos)
{
    for(int iRect=0; iRect<iconrects.size(); iRect++){
        UIRect& rect = iconrects[iRect];
	    if(rect.contains(mousepos)){
            return iRect;
        }
    }
    return -1;
}

std::vector<UIRect> rendertrainicons(
    Rendering* r, InterfaceManager& ui,
    std::vector<WagonInfo>& wagoninfos, 
    UIRect maxrect, int splitid)
{
    Coord iconoffset = 2;
    Coord splitoffset = 0;
	SpriteManager& spritemanager = ui.getgame().getsprites();
    Icon wagonicon;
    wagonicon.ported = false;
    Coord icon_x = 0;
    std::vector<UIRect> iconuirects;

    for(int iWagon = 0; iWagon<wagoninfos.size(); iWagon++){
        WagonInfo& wagoninfo = wagoninfos[iWagon];
        wagonicon.setspritesheet(spritemanager, wagoninfo.iconname);
        if(splitid>=0 && iWagon>splitid)
            splitoffset = 5;
        UIVec iconsize = wagonicon.getuisize(ui.getuirendering());
        UIRect iconrect = {
            maxrect.x + icon_x + splitoffset, 
            maxrect.y, 
            iconsize.x + iconoffset, 
            maxrect.h
        };
        wagonicon.render(r, iconrect);
        iconrect.x-=splitoffset; // the clickable mask should not be offset
        iconuirects.push_back(iconrect);
        icon_x += iconsize.x + iconoffset;
    }
    return iconuirects;
}

}