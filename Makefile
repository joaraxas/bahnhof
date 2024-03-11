

all:
	g++ -std=c++20 src/utils.cpp src/track.cpp src/rollingstock.cpp src/buildings.cpp src/control.cpp src/main.cpp -w -lSDL2 -lSDL2_image -lSDL2_ttf $(shell sdl2-config --cflags --libs) -o build/bahnhof
