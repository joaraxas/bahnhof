#include "bahnhof/ui/ui.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/input/input.h"

using namespace UI;

Tooltip::Tooltip(InputManager& i, UIRendering& u)
    : input(i), uirendering(u) {}

void Tooltip::set(std::string tip)
{
    if(!tips.empty())
        tips += '\n';
    tips += tip;
}

void Tooltip::render(Rendering* r)
{
    if(tips.empty()) return;
    constexpr UIVec margin{2,1};
    UIVec mousepos = uirendering.screentoui(input.screenmousepos());
    UIRect uirect{mousepos.x+20, mousepos.y, 100, 180};
    auto rect = uirendering.gettextsize(tips, uirect, margin.x, margin.y);
    uirendering.renderrectangle(r, rect, UI::TooltipBackground, true);
    uirendering.renderrectangle(r, rect, UI::TooltipBorder, false);
    uirendering.rendertext(
        r, tips, uirect, UI::InvertedInfo, false, margin.x, margin.y);

    tips.clear();
}