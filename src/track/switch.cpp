#include<map>
#include "bahnhof/graphics/rendering.h"
#include "bahnhof/track/track.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/common/gamestate.h"

namespace Tracks
{
Switch::Switch(Node* mynode, Track* track, bool isupordown) : node(mynode), pointsupwards(isupordown)
{
	tracksystem = node->tracksystem;
    id = tracksystem->addswitchtolist(this);
    addtrack(track);
	sprite.setspritesheet(tracksystem->game->getsprites(), sprites::switchsprite);
	sprite.zoomed = false;
    if(pointsupwards)
        sprite.imageangle = node->dir.getradiansup() - Angle(pi/2);
    else
        sprite.imageangle = node->dir.getradiansdown() - Angle(pi/2);
}

Switch::~Switch()
{
    tracksystem->removeswitchfromlist(id);
}

void Switch::addtrack(Track* track){
    int insertionindex = 0;
    if(tracks.size()>=1){
        insertionindex = tracks.size();
        float newtrackcurvature = 1./getradiusoriginatingfromnode(track, node->id);
        for(int iTrack=0; iTrack<tracks.size(); iTrack++){
            if(newtrackcurvature > 1./getradiusoriginatingfromnode(tracks[iTrack], node->id))
                insertionindex = iTrack;
            if(insertionindex==iTrack) break;
        }
    }
    tracks.insert(tracks.begin()+insertionindex, track);
    if(pointsupwards)
        node->trackup = tracks[switchstate];
    else
        node->trackdown = tracks[switchstate];
}

void Switch::removetrack(Track* track){
    if(tracks[switchstate]==track)
        switchstate = 0;
    auto it = find(tracks.begin(), tracks.end(), track);
    if (it != tracks.end())
        tracks.erase(it);
    if(pointsupwards)
        node->trackup = tracks[switchstate];
    else
        node->trackdown = tracks[switchstate];
}

float getradiusoriginatingfromnode(Track* track, nodeid node)
{
	State state(0, 0.5, track->previousnode->id==node);
	return track->getradius(state);
}

Vec Switch::pos(){
    return getswitchpos(node->pos, node->dir, pointsupwards);
}

int Switch::getstateforordergeneration(){
    return switchstate;
}

void Switch::render(Rendering* r)
{
	float scale = r->getcamscale();
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
    if(pointsupwards)
        node->trackup = tracks[switchstate];
    else
        node->trackdown = tracks[switchstate];
}
}