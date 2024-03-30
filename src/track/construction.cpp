#include<iostream>
#include<string>
#include<map>
#include "bahnhof/track/trackinternal.h"
#include "bahnhof/track/track.h"


namespace Tracks{

namespace Construction{
nodeid extendtracktopos(Tracksystem& tracksystem, nodeid fromnode, Vec pos)
{
	Vec posdiff = pos - tracksystem.getnode(fromnode)->getpos();
	float dir = truncate(2*atan2(-posdiff.y,posdiff.x) - tracksystem.getnode(fromnode)->getdir());
	nodeid newnode = tracksystem.addnode(pos, dir);
	tracksystem.addtrack(fromnode, newnode);
	return newnode;
}

void connecttwonodes(Tracksystem& tracksystem, nodeid node1, nodeid node2)
{
	if(node1==node2)
		return; // TODO: Also stop other illegal connections, like where track already exists
	Vec newnodepoint;
	Node* node1pointer = tracksystem.getnode(node1);
	Node* node2pointer = tracksystem.getnode(node2);
	Vec pos1 = node1pointer->getpos();
	Vec pos2 = node2pointer->getpos();
	float y1 = -pos1.y;
	float y2 = -pos2.y;
	float x1 = pos1.x;
	float x2 = pos2.x;
	float tanth1 = tan(node1pointer->getdir());
	float tanth2 = tan(node2pointer->getdir());

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
		nodeid newnode = extendtracktopos(tracksystem, node1, newnodepoint);
		tracksystem.addtrack(newnode, node2);
	}
	else
		tracksystem.addtrack(node1, node2);
}
}
}