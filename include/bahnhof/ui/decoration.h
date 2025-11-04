#pragma once
#include "bahnhof/common/math.h"
#include "bahnhof/ui/ui.h"

class Rendering;
class Train;
struct TrainInfo;
struct WagonInfo;

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
    static constexpr int margin_x = 2;
    static constexpr int margin_y = 1;
};

class EditableText : public Text
{
public:
    EditableText(Host* p, std::string& t, UIRect r);
    ~EditableText();
    void leftclick(UIVec mousepos);
    void render(Rendering*);
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

class TrainIcons : public Element
{
public:
    TrainIcons(Host* p, UIRect maxarea, Train& t) : Element(p), train(t) {rect = maxarea;};
    void render(Rendering*);
    void mousehover(UIVec pos, int ms);
    void leftclick(UIVec mousepos);
private:
    int getwagonidatmousepos(UIVec mousepos);
    Train& train;
    std::vector<UIRect> iconrects;
    int rendersplitafterwagonid = -1;
};

std::vector<UIRect> rendertrainicons(
    Rendering* r, InterfaceManager& ui, 
    std::vector<WagonInfo>& wagoninfos, UIRect rect, 
    int splitid=-1);

}
