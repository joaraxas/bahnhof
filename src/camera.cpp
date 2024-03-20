#include<iostream>
#include "bahnhof/common/camera.h"
#include "bahnhof/common/math.h"
#include "bahnhof/common/rendering.h"

Camera::Camera(){
    cam = {0,0,SCREEN_WIDTH, SCREEN_HEIGHT};
}

Vec Camera::mapcoord(Vec screenpos){
    return Vec(screenpos.x/scale+cam.x, screenpos.y/scale+cam.y);
}

Vec Camera::screencoord(Vec mappos){
    return Vec((mappos.x-cam.x)*scale, (mappos.y-cam.y)*scale);
}

void Camera::zoomin(Vec centerpoint){
    cam.x+=cam.w/2*centerpoint.x/SCREEN_WIDTH;
    cam.w/=2;
    cam.y+=cam.h/2*centerpoint.y/SCREEN_HEIGHT;
    cam.h/=2;
    scale*=2;
    restricttomap();
}

void Camera::zoomout(Vec centerpoint){
    if(2*cam.w<=MAP_WIDTH && 2*cam.h<=MAP_HEIGHT){
        cam.x-=cam.w*centerpoint.x/SCREEN_WIDTH;
        cam.w*=2;
        cam.y-=cam.h*centerpoint.y/SCREEN_HEIGHT;
        cam.h*=2;
        scale/=2;
        std::cout<<scale<<std::endl;
        std::cout<<SCREEN_WIDTH/double(MAP_WIDTH)<<std::endl;
        std::cout<<SCREEN_HEIGHT/double(MAP_HEIGHT)<<std::endl;
        restricttomap();
    }
}

void Camera::pan(Vec direction){
    direction.x = int(direction.x/scale);
    direction.y = int(direction.y/scale);

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
    return scale;
}