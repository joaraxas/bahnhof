#include<iostream>
#include<string>
#include<map>
#include "bahnhof/common/orientation.h"
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"
#include "bahnhof/routing/routing.h"
#include "bahnhof/rollingstock/rollingstock.h"

namespace Tracks{
namespace Construction{

Tracksection extendtracktopos(Tracksystem& tracksystem, Vec frompos, Vec topos)
{
	Vec posdiff = topos - frompos;
	Tangent dir(posdiff);
	Node* fromnode = new Node(tracksystem, frompos, dir, -1);
	Tracksection newsection = Construction::extendtracktopos(tracksystem, fromnode, topos);
	newsection = newsection + Tracksection({},{fromnode});
	return newsection;
}

Tracksection extendtracktopos(Tracksystem& tracksystem, Node* fromnode, Vec topos)
{
	Tangent dir = gettangentatpointoncurvestartingfromnode(*fromnode, topos);
    Node* tonodepointer = new Node(tracksystem, topos, dir, -1);
    Track* newtrack = new Track(tracksystem, *fromnode, *tonodepointer, -1);
	Tracksection section({newtrack}, {tonodepointer});
	return section;
}

Tracksection connecttwonodes(Tracksystem& tracksystem, Node* node1, Node* node2)
{
	if(node1==node2)
		return Tracksection({},{}); // TODO: Also stop other illegal connections, like where track already exists
	Vec newnodepoint;
	Vec pos1 = node1->getpos();
	Vec pos2 = node2->getpos();
	float y1 = -pos1.y;
	float y2 = -pos2.y;
	float x1 = pos1.x;
	float x2 = pos2.x;
	float tanth1 = tan(node1->getdir());
	float tanth2 = tan(node2->getdir());

	float intersectx = (y2-y1+x1*tanth1 - x2*tanth2)/(tanth1 - tanth2); // TODO: This looks like it will fail if nodes are parallel
	float intersecty = -(y1 + (intersectx - x1)*tanth1);
	if(abs(tanth1)>1e5){
		intersectx = x1;
		intersecty = -(y2 + (intersectx - x2)*tanth2);
	}
	Vec tangentintersection(intersectx, intersecty);
	float disttointersect1 = distancebetween(pos1, tangentintersection);
	float disttointersect2 = distancebetween(pos2, tangentintersection);
	Tangent dir;
	if(disttointersect1 > disttointersect2){
		newnodepoint = tangentintersection + (pos1 - tangentintersection)/disttointersect1*disttointersect2;
		dir = gettangentatpointoncurvestartingfromnode(*node2, newnodepoint);
	}
	else{
		newnodepoint = tangentintersection + (pos2 - tangentintersection)/disttointersect2*disttointersect1;
		dir = gettangentatpointoncurvestartingfromnode(*node1, newnodepoint);
	}
	if(
		(distancebetween(pos1, newnodepoint) < 10 && dir.absanglediff(node1->getdir()) < 5.0/180.0*pi) ||
		(distancebetween(pos2, newnodepoint) < 10 && dir.absanglediff(node2->getdir()) < 5.0/180.0*pi)
	){
		Track* newtrack = new Track(tracksystem, *node1, *node2, -1);
		return Tracksection({newtrack},{});
	}
	Tracksection section = extendtracktopos(tracksystem, node1, newnodepoint);
	section.tracks.push_back(new Track(tracksystem, *section.nodes.back(), *node2, -1));
	return section;
}

void splittrack(Tracksystem& tracksystem, Node* node, State state)
{
	Track* trackpointer = tracksystem.gettrack(state.track);
	Node* previousnode = trackpointer->previousnode;
	Node* nextnode = trackpointer->nextnode;
	
	Track* track1 = new Track(tracksystem, *previousnode, *node, -1);
	Track* track2 = new Track(tracksystem, *node, *nextnode, -1);
	tracksystem.addtrack(*track1);
	tracksystem.addtrack(*track2);

	tracksystem.references->movereferenceswhentracksplits(state, *track1, *track2);

	tracksystem.removetrack(state.track);
}	

Tangent gettangentatpointoncurvestartingfromnode(Node& startnode, Vec topos)
{
	Vec posdiff = topos - startnode.getpos();
	return Tangent(-startnode.getdir() + 2*Angle(posdiff));
}

}
}