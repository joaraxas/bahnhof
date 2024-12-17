#include<iostream>
#include "bahnhof/common/camera.h"
#include "bahnhof/common/timing.h"
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/common/gamestate.h"

Camera::Camera(Game* whatgame){
    game = whatgame;
    Vec viewsize = game->getrendering().getviewsize();
    cam = {0,0,int(viewsize.x), int(viewsize.y)};
}

Vec Camera::mapcoord(Vec screenpos){
    return Vec(screenpos.x/getscale()+cam.x, screenpos.y/getscale()+cam.y);
}

Vec Camera::screencoord(Vec mappos){
    return Vec((mappos.x-cam.x)*getscale(), (mappos.y-cam.y)*getscale());
}

void Camera::zoomin(Vec centerpoint){
    Vec viewsize = game->getrendering().getviewsize();
    cam.x+=cam.w/2*centerpoint.x/viewsize.x;
    cam.w/=2;
    cam.y+=cam.h/2*centerpoint.y/viewsize.y;
    cam.h/=2;
    logscale++;
    game->gettimemanager().speeddown();
    restricttomap();
}

void Camera::zoomout(Vec centerpoint){
    if(2*cam.w<=MAP_WIDTH && 2*cam.h<=MAP_HEIGHT){
        Vec viewsize = game->getrendering().getviewsize();
        cam.x-=cam.w*centerpoint.x/viewsize.x;
        cam.w*=2;
        cam.y-=cam.h*centerpoint.y/viewsize.y;
        cam.h*=2;
        logscale--;
        game->gettimemanager().speedup();
        restricttomap();
    }
}

void Camera::pan(Vec direction){
    direction.x = int(direction.x/getscale());
    direction.y = int(direction.y/getscale());

    cam.x = cam.x + direction.x;
    cam.y = cam.y + direction.y;
    restricttomap();
}

void Camera::restricttomap(){
    if(cam.x<0) cam.x = 0;
    if(cam.x+cam.w>MAP_WIDTH) cam.x = MAP_WIDTH - cam.w;

    if(cam.y<0) cam.y = 0;
    if(cam.y+cam.h>MAP_HEIGHT) cam.y = MAP_HEIGHT - cam.h;
}

float Camera::getscale(){
    return pow(2, logscale);
}