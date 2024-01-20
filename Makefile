

all:
	g++ -std=c++17 utils.cpp track.cpp rollingstock.cpp buildings.cpp control.cpp main.cpp -w -lSDL2 -lSDL2_image -o train
