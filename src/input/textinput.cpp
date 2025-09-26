#include "bahnhof/input/textinput.h"
#include "bahnhof/ui/decoration.h"


void TextInputManager::starttextinput(UI::EditableText* textobject)
{
    if(!iswriting()){
        SDL_StartTextInput();
        editingtextobject = textobject;
        editingtextobject->startwriting();
    }
}

void TextInputManager::savetext()
{
    if(iswriting()){
        editingtextobject->updatesource();
        endtextinput();
    }
}

void TextInputManager::endtextinput()
{
    if(iswriting()){
        editingtextobject->stopwriting();
        editingtextobject = nullptr;
        SDL_StopTextInput();
    }
}

bool TextInputManager::handle(SDL_Event& e)
{
    if(iswriting()){
        switch(e.type){
        case SDL_KEYDOWN:{
            switch(e.key.keysym.sym){
            case SDLK_ESCAPE:{
                endtextinput();
                break;
            }
            case SDLK_RETURN:{
                savetext();
                break;
            }
            case SDLK_BACKSPACE:{
                editingtextobject->deleteselection();
                break;
            }
            case SDLK_LEFT:{
                editingtextobject->movecursorleft();
                break;
            }
            case SDLK_RIGHT:{
                editingtextobject->movecursorright();
                break;
            }
            }
            return true;
        }
        case SDL_TEXTINPUT:{
            editingtextobject->addtext(e.text.text);
            return true;
        }
        default:
            return false;
        }
    }
    return false;
}
