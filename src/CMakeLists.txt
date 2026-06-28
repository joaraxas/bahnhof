add_library(bahnhof_lib
    math/math.cpp
    math/rectangle.cpp
    math/rotatedrectangle.cpp
    math/ringsector.cpp
    math/intersection.cpp
    math/orientation.cpp
    graphics/rendering.cpp
    graphics/graphics.cpp
    graphics/sprite.cpp
    graphics/spritesheet.cpp
    graphics/spritemanager.cpp
    game/camera.cpp
    game/timing.cpp
    game/gamestate.cpp
    game/game.cpp
    game/resources.cpp
    input/input.cpp
    input/controlmanager.cpp
    input/textinput.cpp
    input/inputmodes.cpp
    input/builder.cpp
    input/trackbuilder.cpp
    input/signalbuilder.cpp
    input/buildingbuilder.cpp
    ui/ui.cpp
    ui/uirendering.cpp
    ui/tooltip.cpp
    ui/host.cpp
    ui/panel.cpp
    ui/element.cpp
    ui/layouts.cpp
    ui/decoration.cpp
    ui/button.cpp
    ui/table.cpp
    ui/tableline.cpp
    track/track.cpp
    track/tracksystem.cpp
    track/trackreferencing.cpp
    track/trackinterface.cpp
    track/trackinput.cpp
    track/construction.cpp
    track/node.cpp
    track/switch.cpp
    track/signal.cpp
    routing/routemanager.cpp
    routing/route.cpp
    routing/order.cpp
    rollingstock/rollingstock.cpp
    rollingstock/train.cpp
    rollingstock/trainmanager.cpp
    rollingstock/rollingstockmanager.cpp
    buildings/buildings.cpp
    buildings/buildingmanager.cpp
    economy/economy.cpp
    economy/economymanager.cpp
    economy/slogans.cpp
)

target_include_directories(bahnhof_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(localization)

target_link_libraries(bahnhof_lib PUBLIC
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    localization_lib
)

target_compile_features(bahnhof_lib PUBLIC cxx_std_20)

target_compile_options(bahnhof_lib PUBLIC -w)

add_executable(bahnhof
    main.cpp
)

target_link_libraries(bahnhof PRIVATE
    bahnhof_lib
)

add_custom_command(TARGET bahnhof POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)