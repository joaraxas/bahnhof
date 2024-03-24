#include<iostream>
#include<string>
#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/common/gamestate.h"

Switch::Switch(Node* mynode, trackid track, bool isupordown) : node(mynode), updown(isupordown)
{
	tracksystem = node->tracksystem;
    id = tracksystem->addswitchtolist(this);
    addtrack(track);
	sprite.setspritesheet(tracksystem->game->getsprites(), sprites::switchsprite);
	sprite.zoomed = false;
	sprite.imageangle = node->dir - pi/2;
    if(!updown) sprite.imageangle+=pi;
}

void Switch::addtrack(trackid track){
    int insertionindex = 0;
    if(tracks.size()>=1){
        insertionindex = tracks.size();
        float newtrackcurvature = 1./getradiusoriginatingfromnode(*tracksystem, node->id, track);
        for(int iTrack=0; iTrack<tracks.size(); iTrack++){
            if(newtrackcurvature < 1./getradiusoriginatingfromnode(*tracksystem, node->id, tracks[iTrack]))
                insertionindex = iTrack;
            if(insertionindex==iTrack) break;
        }
    }
    tracks.insert(tracks.begin()+insertionindex, track);
    if(updown)
        node->trackup = tracks[switchstate];
    else
        node->trackdown = tracks[switchstate];
}

float getradiusoriginatingfromnode(Tracksystem& t, nodeid node, trackid track)
{
	Track* trackpointer = t.gettrack(track);
	State state(track, 0.5, trackpointer->previousnode==node);
	return t.getradius(state);
}

Vec Switch::pos(){
    return getswitchpos(node->pos, node->dir, updown);
}

int Switch::getstateforordergeneration(){
    return switchstate;
}

void Switch::render(Rendering* r)
{
	float scale = r->getscale();
	//if(scale>0.3){
        Vec switchpos = pos();
        sprite.imagetype = fmin(1, switchstate);
        sprite.render(r, switchpos);
        if(!nicetracks)
            r->rendertext(std::to_string(switchstate), switchpos.x, switchpos.y+7, {0,0,0,0});
	//}
}

void Switch::setswitch(int newstate)
{
	if(newstate==-1){
        switchstate++;
        if(switchstate>=tracks.size())
            switchstate = 0;
    }
	else{
        if(newstate>=0 && newstate<tracks.size())
		    switchstate = newstate;
        else
            std::cout<<"Error: tried to set switch "<<id<<" to illegal state " << newstate<<std::endl;
    }
    if(updown)
        node->trackup = tracks[switchstate];
    else
        node->trackdown = tracks[switchstate];
}
