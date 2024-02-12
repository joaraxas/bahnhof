

all:
	g++ -std=c++20 utils.cpp track.cpp rollingstock.cpp buildings.cpp control.cpp main.cpp -w -lSDL2 -lSDL2_image -lSDL2_ttf $(shell sdl2-config --cflags --libs) -o train
