cmake_minimum_required(VERSION 3.12)
project(bahnhof VERSION 1.0)

# If on Windows, set these paths to wherever you have SDL installed
set(SDL2_PATH "C:\\mingw_dev_lib\\SDL2")
set(SDL2_IMAGE_PATH "C:\\mingw_dev_lib\\SDL2")
set(SDL2_TTF_PATH "C:\\mingw_dev_lib\\SDL2")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${bahnhof_SOURCE_DIR}/cmake")

add_executable(bahnhof
    src/main.cpp
    src/math/math.cpp
    src/math/rectangle.cpp
    src/math/rotatedrectangle.cpp
    src/math/ringsector.cpp
    src/math/intersection.cpp
    src/math/orientation.cpp
    src/graphics/rendering.cpp
    src/graphics/graphics.cpp
    src/graphics/sprite.cpp
    src/graphics/spritesheet.cpp
    src/graphics/spritemanager.cpp
    src/game/camera.cpp
    src/game/timing.cpp
    src/game/gamestate.cpp
    src/game/game.cpp
    src/game/resources.cpp
    src/input/input.cpp
    src/input/textinput.cpp
    src/input/builder.cpp
    src/input/trackbuilder.cpp
    src/input/signalbuilder.cpp
    src/input/buildingbuilder.cpp
    src/ui/ui.cpp
    src/ui/uirendering.cpp
    src/ui/tooltip.cpp
    src/ui/host.cpp
    src/ui/panel.cpp
    src/ui/element.cpp
    src/ui/layouts.cpp
    src/ui/decoration.cpp
    src/ui/button.cpp
    src/ui/table.cpp
    src/ui/tableline.cpp
    src/track/track.cpp
    src/track/tracksystem.cpp
    src/track/trackreferencing.cpp
    src/track/trackinterface.cpp
    src/track/trackinput.cpp
    src/track/construction.cpp
    src/track/node.cpp
    src/track/switch.cpp
    src/track/signal.cpp
    src/routing/routemanager.cpp
    src/routing/route.cpp
    src/routing/order.cpp
    src/rollingstock/rollingstock.cpp
    src/rollingstock/train.cpp
    src/rollingstock/trainmanager.cpp
    src/rollingstock/rollingstockmanager.cpp
    src/buildings/buildings.cpp
    src/buildings/buildingmanager.cpp
)

target_include_directories(bahnhof PRIVATE include)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(bahnhof ${SDL2_LIBRARY}
                             ${SDL2_IMAGE_LIBRARIES}
                             ${SDL2_TTF_LIBRARIES})

set_target_properties(bahnhof PROPERTIES CXX_STANDARD 20)

target_compile_options(bahnhof PRIVATE -w)
