#pragma once
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include "math.h"

namespace UI{
    class EditableText;
}

class InputManager;

class TextInputManager
{
public:
    TextInputManager(InputManager& owner) : input(owner) {};
    void starttextinput(UI::EditableText* textobject);
    void savetext();
    void endtextinput();
    bool handle(SDL_Event& e);
    bool iswriting() {return editingtextobject!=nullptr;};
private:
    InputManager& input;
    UI::EditableText* editingtextobject = nullptr;
};
