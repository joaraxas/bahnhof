#include "bahnhof/common/shape.h"
#include "bahnhof/graphics/rendering.h"

AnnularSector::AnnularSector(Vec frompos, Angle fromdir, Angle arcangle, float r, float thickness)
{
	float radius = abs(r);
	angle = arcangle;
	midpoint = globalcoords(Localvec{0,radius}, fromdir, frompos);
	innerradius = radius - 0.5*thickness;
	outerradius = radius + 0.5*thickness;
	rightlimitangle = fromdir - Angle(pi/2);
	nSegments = discretizecurve(angle, outerradius);
}

void AnnularSector::renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed) const
{
	std::vector<Vec> vertvecs = getvertices();
	std::vector<int> indices;
	indices.reserve(2*3*nSegments);
	for(int i=0; i<nSegments*2; i++){
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+2);
	}
	r->renderfilledpolygon(vertvecs, indices, color, ported, zoomed);
	
	// r->renderfilledrectangle({int(midpoint.x-3),int(midpoint.y-3),6,6});
}

Vec AnnularSector::mid() const
{
	return midpoint;
}

Angle AnnularSector::getorientation() const
{
	return rightlimitangle + angle * 0.5;
}

std::vector<Vec> AnnularSector::getvertices() const
{
	std::vector<Vec> verts;
	verts.reserve(nSegments*2+2);
	const Angle increment = angle/nSegments;
	for(int i=0; i<nSegments+1; i++){
		Angle alpha = rightlimitangle + i*increment;
		verts.emplace_back(globalcoords({innerradius, 0}, alpha, midpoint));
		verts.emplace_back(globalcoords({outerradius, 0}, alpha, midpoint));
	}
	return verts;
}

bool AnnularSector::contains(Vec point) const
{
	Vec diff = point-midpoint;
	float distance = norm(diff);
	if(distance>outerradius) return false;
	if(distance<innerradius) return false;
	Angle angletomidpoint(diff);
	if(angletomidpoint.isbetween(rightlimitangle, rightlimitangle+angle))
		return true;
	return false;
}

bool AnnularSector::intersects(const Shape& shape) const
{
	return shape.intersectsannularsector(*this);
}

bool AnnularSector::intersectsrect(const Rectangle& shape) const
{
	// First check whether one point from either body is inside the other.
	// This is to catch the case when one shape in entirely enclosed by the other. If so we can't use edges.
	Vec rightoutercorner(midpoint.x+outerradius*cos(-rightlimitangle), midpoint.y+outerradius*sin(-rightlimitangle));
	if(shape.contains(rightoutercorner))
		return true;
	auto othervertices = shape.getvertices();
	if(contains(othervertices.front()))
		return true;

	return intersectsanyedge(othervertices);
}

bool AnnularSector::intersectsrotrect(const RotatedRectangle& shape) const
{
	// First check whether one point from either body is inside the other.
	// This is to catch the case when one shape in entirely enclosed by the other. If so we can't use edges.
	Vec rightoutercorner(midpoint.x+outerradius*cos(-rightlimitangle), midpoint.y+outerradius*sin(-rightlimitangle));
	if(shape.contains(rightoutercorner))
		return true;
	auto othervertices = shape.getvertices();
	if(contains(othervertices.front()))
		return true;

	return intersectsanyedge(othervertices);
}

bool AnnularSector::intersectsannularsector(const AnnularSector&) const
{
	return false; // for now this won't happen
}

bool AnnularSector::intersectsanyedge(const std::vector<Vec>& orderedvertices) const
{
	std::vector<Edge> edges;
	edges.reserve(orderedvertices.size());
	for(int i=0; i<orderedvertices.size(); i++){
		int nexti = (i+1) % orderedvertices.size();
		edges.push_back(Edge(orderedvertices[i], orderedvertices[nexti]));
		
		// check intersection with both arcs
		Arc outerarc(midpoint, outerradius, rightlimitangle, angle);
		if(Intersection::edgeintersectsarc(edges.back(), outerarc))
			return true;
		Arc innerarc(midpoint, innerradius, rightlimitangle, angle);
		if(Intersection::edgeintersectsarc(edges.back(), innerarc))
			return true;
	}

	// This sector has only two straight edges. We need to check whether any of these side edges collide with the rectangle's.
	std::vector<Edge> myedges;
	myedges.reserve(2);
	Vec rightinnercorner(midpoint.x+innerradius*cos(-rightlimitangle), midpoint.y+innerradius*sin(-rightlimitangle));
	Vec rightoutercorner(midpoint.x+outerradius*cos(-rightlimitangle), midpoint.y+outerradius*sin(-rightlimitangle));
	myedges.push_back(Edge(rightinnercorner, rightoutercorner));
	Vec leftinnercorner(midpoint.x+innerradius*cos(-rightlimitangle-angle), midpoint.y+innerradius*sin(-rightlimitangle-angle));
	Vec leftoutercorner(midpoint.x+outerradius*cos(-rightlimitangle-angle), midpoint.y+outerradius*sin(-rightlimitangle-angle));
	myedges.push_back(Edge(leftinnercorner, leftoutercorner));
	if(Intersection::anyedgesintersect(myedges, edges))
		return true;
	return false;
}
