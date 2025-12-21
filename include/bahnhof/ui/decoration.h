#pragma once
#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/ui/element.h"
#include "bahnhof/ui/uistyle.h"

class Rendering;
class Train;
struct TrainInfo;
struct WagonInfo;
class WagonFactory;

namespace UI{

class Text : public Element
{
public:
    Text(Host* p, std::string t, UIRect r);
    void render(Rendering*);
    std::string text;
    SDL_Color color = {0,0,0,255};
    bool centered = true;
    TextStyle style = Info;
    static constexpr Coord margin_x = 2;
    static constexpr Coord margin_y = 1;
};

class EditableText : public Text
{
public:
    EditableText(Host* p, std::string& t, UIRect r={0, 0, 50, 20});
    ~EditableText();
    void leftclick(UIVec mousepos);
    void render(Rendering*);
    UIVec getminimumsize() {return {originalrect.w+getpadding().x*2, originalrect.h+getpadding().y*2};};
    UIRect place(UIRect r);
    bool resizable_x() const override {return true;};
    void updatesource();
    void startwriting();
    void stopwriting();
    void deleteselection();
    void addtext(const std::string& string);
    void movecursorleft();
    void movecursorright();
private:
    void updatewritingarea();
    std::string& textreference; // TODO: This is currently safe but will 
                                // break if the referred string is destroyed 
                                // without removing the panel with the editable text
    std::string fallbacktext;
    std::string shortenedtext;
    bool beingedited;
    int cursorindex;
    UIRect originalrect;
};

class TrainCoupler : public Element
{
public:
    TrainCoupler(Host* p, Train& t, UIRect maxarea={0,0,200,20}) : Element(p), train(t) {rect = maxarea;};
    void render(Rendering*);
    void mousehover(UIVec pos, int ms);
    void leftclick(UIVec mousepos);
private:
    int getwagonidatmousepos(UIVec mousepos);
    Train& train;
    std::vector<UIRect> iconrects;
    int rendersplitafterwagonid = -1;
};

class WagonQueue : public Element
{
public:
    WagonQueue(Host* h, WagonFactory& f, UIRect maxarea={0,0,150,20}) : 
        Element{h}, factory{f} {rect = maxarea;};
    void render(Rendering* r);
    void leftclick(UIVec mousepos);
private:
    int getwagonidatmousepos(UIVec mousepos);
    WagonFactory& factory;
    std::vector<UIRect> iconrects;
};

std::vector<UIRect> rendertrainicons(
    Rendering* r, InterfaceManager& ui, 
    std::vector<WagonInfo>& wagoninfos, UIRect rect, 
    int splitid=-1);

}
