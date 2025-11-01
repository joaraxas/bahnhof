#pragma once
#include "bahnhof/common/forwardincludes.h"

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
