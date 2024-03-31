#include<iostream>
#include<string>
#include<map>
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"


namespace Tracks{
namespace Construction{

Tracksection extendtracktopos(Tracksystem& tracksystem, Node* fromnode, Vec pos)
{
	Vec posdiff = pos - fromnode->getpos();
	float dir = truncate(2*atan2(-posdiff.y,posdiff.x) - fromnode->getdir());
    Node* tonodepointer = new Node(tracksystem, pos, dir, -1);
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

	float intersectx = (y2-y1+x1*tanth1 - x2*tanth2)/(tanth1 - tanth2);
	float intersecty = -(y1 + (intersectx - x1)*tanth1);
	if(abs(tanth1)>1e5){
		intersectx = x1;
		intersecty = -(y2 + (intersectx - x2)*tanth2);
	}
	Vec tangentintersection(intersectx, intersecty);
	float disttointersect1 = distancebetween(pos1, tangentintersection);
	float disttointersect2 = distancebetween(pos2, tangentintersection);
	if(disttointersect1 > disttointersect2)
		newnodepoint = tangentintersection + (pos1 - tangentintersection)/disttointersect1*disttointersect2;
	else
		newnodepoint = tangentintersection + (pos2 - tangentintersection)/disttointersect2*disttointersect1;
	if(distancebetween(pos1, newnodepoint)> 10 && distancebetween(pos2, newnodepoint)> 10){ //TODO: bug when connecting two nodes where one is in plane of other but directions not parallel
		Tracksection section = extendtracktopos(tracksystem, node1, newnodepoint);
		section.tracks.push_back(new Track(tracksystem, *section.nodes.back(), *node2, -1));
		return section;
	}
	Track* newtrack = new Track(tracksystem, *node1, *node2, -1);
	return Tracksection({newtrack},{});
}

void splittrack(Tracksystem& tracksystem, Node* node, State state)
{
	Track* trackpointer = tracksystem.gettrack(state.track);
	Node* previousnode = trackpointer->previousnode;
	Node* nextnode = trackpointer->nextnode;
	Tracksection section1 = connecttwonodes(tracksystem, previousnode, node);
	Tracksection section2 = connecttwonodes(tracksystem, node, nextnode);
	Track* track1 = section1.tracks[0];
	Track* track2 = section2.tracks[0];
	tracksystem.addtrack(*track1);
	tracksystem.addtrack(*track2);
	trackpointer->split(*track1, *track2, state);
	tracksystem.removetrack(state.track);
}	


}
}