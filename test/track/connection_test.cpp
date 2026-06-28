#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/track/track.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"

using namespace Tracks;


int main(int argv, char** argc) {
	init();
    Game game{};

    Tracksystem& tracks = game.getgamestate().gettracksystems();
    Angle angle;
    Tracksection s = Input::planconstructionto(tracks, Vec{500,400}, Vec{900,400});
    Input::buildsection(tracks, s);
    s = Input::planconstructionto(tracks, Vec{500,400}, 400, angle);
    Input::buildsection(tracks, s);
    
    Vec a{700,500};
    s = Input::planconstructionto(tracks, a, Vec{900,500});
    Input::buildsection(tracks, s);
    s = Input::planconstructionto(tracks, tracks.getnode(Input::selectnodeat(tracks, a)), Vec{100,499.9});
    Input::buildsection(tracks, s);

	game.play();
	close();
}