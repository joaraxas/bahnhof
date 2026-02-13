#pragma once
#include "bahnhof/common/math.h"

class Rendering;

class InputMode
{
public:
    virtual ~InputMode() {};
    virtual void render(Rendering*) = 0;
    virtual void leftclickmap(Vec mappos) = 0;
    virtual void rightclickmap(Vec mappos) = 0;
    virtual void leftreleasedmap(Vec mappos) = 0;
};
