#include "bahnhof/common/forwardincludes.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/graphics/graphics.h"
#include "bahnhof/common/gamestate.h"

using namespace Tracks;


int main(int argv, char** argc) {
	init();
    Game game{};

    Tracksystem& tracks = game.getgamestate().gettracksystems();
    // tracks.buildfromcoords({{500,400}, 
    //                         {400,400}});
    Angle angle;
    Tracksection s = Input::planconstructionto(tracks, Vec{500,400}, Vec{900,400});
    std::cout<<s.nodes.front()->getpos()<<std::endl;
    std::cout<<s.nodes.back()->getpos()<<std::endl;
    Input::buildsection(tracks, s);
    s = Input::planconstructionto(tracks, Vec{500,400}, 400, angle);
    std::cout<<angle<<std::endl;
    std::cout<<s.nodes.back()->getpos()<<std::endl;
    Input::buildsection(tracks, s);
    // auto s = Input::planconstructionto(tracks, Vec{400,400}, Vec{400,500});
    // Input::buildsection(tracks, s);
    // s = Input::planconstructionto(tracks, Input::getendpointat(tracks, Vec{400,400}), Vec{400,300});
    // Input::buildsection(tracks, s);

	game.play();
	close();
}