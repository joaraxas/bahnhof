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
	if(posdiff.iszero()) return Tracksection{};
	Tangent dir(posdiff);
	Node* fromnode = new Node(tracksystem, frompos, dir, -1);
	Tracksection newsection = Construction::extendtracktopos(tracksystem, fromnode, topos);
	newsection = newsection + Tracksection({},{fromnode});
	return newsection;
}

Tracksection extendtracktopos(Tracksystem& tracksystem, Node* fromnode, Vec topos)
{
	Tangent dir = gettangentatpointoncurvestartingfromnode(*fromnode, topos);
	Node* tonode = new Node(tracksystem, topos, dir, -1);
	Track* newtrack = new Track(tracksystem, *fromnode, *tonode, -1);
	Tracksection section({newtrack}, {tonode});
	return section;
}

Tracksection connecttwonodes(Tracksystem& tracksystem, Node* node1, Node* node2)
{
	if(node1==node2)
		return Tracksection({},{}); // TODO: Also stop other illegal connections, like where track already exists

	Vec newnodepoint;
	Tangent newdir;
	Vec pos1 = node1->getpos();
	Angle angle1 = node1->getdir().getradiansup();
	Vec pos2 = node2->getpos();
	Localvec p2 = localcoords(pos2, angle1, pos1);
	float tandir2 = tan(node2->getdir() - angle1);
	if(abs(tandir2)<1e-4){ //nodes are parallel
		if(abs(p2.y)<1){
			Track* newtrack = new Track(tracksystem, *node1, *node2, -1);
			return Tracksection({newtrack},{});
		}
		newnodepoint = globalcoords(p2*0.5, angle1, pos1);
		newdir = gettangentatpointoncurvestartingfromnode(*node1, newnodepoint);
	}
	else{
		Localvec intersection_local(p2.x-p2.y/tandir2, 0);
		Vec tangentintersection = globalcoords(intersection_local, angle1, pos1);

		float disttointersect1 = abs(intersection_local.x);
		float disttointersect2 = distancebetween(pos2, tangentintersection);
		if(disttointersect1 > disttointersect2){
			newnodepoint = tangentintersection + (pos1 - tangentintersection)/disttointersect1*disttointersect2;
			newdir = node1->getdir();
		}
		else{
			newnodepoint = tangentintersection + (pos2 - tangentintersection)/disttointersect2*disttointersect1;
			newdir = node2->getdir();
		}
		if(
			(distancebetween(pos1, newnodepoint) < 10 && newdir.absanglediff(node1->getdir()) < 5.0/180.0*pi) ||
			(distancebetween(pos2, newnodepoint) < 10 && newdir.absanglediff(node2->getdir()) < 5.0/180.0*pi)
		){
			Track* newtrack = new Track(tracksystem, *node1, *node2, -1);
			return Tracksection({newtrack},{});
		}
	}
	Node* tonode = new Node(tracksystem, newnodepoint, newdir, -1);
	Track* newtrack1 = new Track(tracksystem, *node1, *tonode, -1);
	Track* newtrack2 = new Track(tracksystem, *tonode, *node2, -1);
	Tracksection section({newtrack1, newtrack2}, {tonode});
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

	tracksystem.references.movereferenceswhentracksplits(state, *track1, *track2);

	tracksystem.removetrack(state.track);
}	

Tangent gettangentatpointoncurvestartingfromnode(Node& startnode, Vec topos)
{
	Vec posdiff = topos - startnode.getpos();
	return Tangent(-startnode.getdir() + 2*Angle(posdiff));
}

}
}