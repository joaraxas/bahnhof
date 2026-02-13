#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/input/input.h"

using namespace UI;

Tooltip::Tooltip(UIRendering& u)
    : uirendering(u) {}

void Tooltip::add(std::string tip)
{
    if(!tips.empty())
        tips += '\n';
    tips += tip;
}

void Tooltip::render(Rendering* r)
{
    if(tips.empty()) return;
    constexpr UIVec margin{3,2};
    UIVec mousepos = uirendering.screentoui(screenmousepos());
    UIRect uirect{mousepos.x+20, mousepos.y, 200, 180};
    auto rect = uirendering.gettextsize(tips, uirect, margin.x, margin.y);
    uirendering.renderrectangle(r, rect, UI::TooltipBackground, true);
    uirendering.renderrectangle(r, rect, UI::TooltipBorder, false);
    uirendering.rendertext(
        r, tips, uirect, UI::InvertedInfo, false, margin.x, margin.y);

    tips.clear();
}