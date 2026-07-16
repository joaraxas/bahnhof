#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"
#include "bahnhof/ui/ui.h"
#include "bahnhof/ui/panels.h"
#include "bahnhof/ui/decoration.h"

using namespace UI;

int main(int argv, char** argc) {
	init();
    Game game{};
    auto& ui = game.getui();

    Panel* panel = new Panel(&ui);
    std::string str{"kö'öööööääååäö"};
    EditableText* text = new EditableText(panel, str, {0,0,50,20});
    panel->addelement(text);

	game.play();
	close();
}