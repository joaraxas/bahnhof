cmake_minimum_required(VERSION 3.12)
project(bahnhof VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${bahnhof_SOURCE_DIR}/cmake")

add_executable(bahnhof
    src/utils.cpp
    src/track.cpp
    src/rollingstock.cpp
    src/buildings.cpp
    src/control.cpp
    src/main.cpp
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
