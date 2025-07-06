#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
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
    Text(Host* p, std::string t, SDL_Rect r);
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
    EditableText(Host* p, std::string& t, SDL_Rect r);
    ~EditableText();
    void leftclick(Vec mousepos);
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
    SDL_Rect originalrect;
};

class TrainIcons : public Element
{
public:
    TrainIcons(Host* p, SDL_Rect maxarea, Train& t) : Element(p), train(t) {rect = maxarea;};
    void render(Rendering*);
    void mousehover(Vec pos, int ms);
    void leftclick(Vec mousepos);
private:
    int getwagonidatmousepos(Vec mousepos);
    Train& train;
    std::vector<SDL_Rect> iconrects;
    int rendersplitafterwagonid = -1;
};

std::vector<SDL_Rect> rendertrainicons(Rendering* r, InterfaceManager& ui, std::vector<WagonInfo>& wagoninfos, SDL_Rect rect, int splitid=-1);

}
