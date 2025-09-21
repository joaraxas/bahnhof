#include<iostream>
#include<string>
#include "bahnhof/common/shape.h"
#include "bahnhof/graphics/rendering.h"

Rectangle::Rectangle(int x, int y, int w, int h) : 
	rect{x, y, w, h}
{}

Rectangle::Rectangle(const SDL_Rect& rect_) : 
	rect(rect_)
{}

Rectangle::Rectangle(Vec pos, int w, int h) : 
	rect{int(pos.x-w*0.5), int(pos.y-h*0.5), w, h}
{}

Rectangle::Rectangle(Vec pos, Vec size) : Rectangle(pos, size.x, size.y) {};

void Rectangle::renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed) const
{
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	r->renderfilledrectangle(rect, ported, zoomed);
}

Vec Rectangle::mid() const
{
	float x_mid = rect.x+rect.w*0.5;
	float y_mid = rect.y+rect.h*0.5;
	return {x_mid, y_mid};
}

bool Rectangle::contains(Vec pos) const
{
	if(pos.x>=rect.x && pos.x<=rect.x+rect.w && pos.y>=rect.y && pos.y<=rect.y+rect.h)
		return true;
	return false;
}

bool Rectangle::intersects(const Shape& shape) const
{
	return shape.intersectsrect(*this);
}

bool Rectangle::intersectsrect(const Rectangle& shape) const
{
	const SDL_Rect& b = shape.rect;
	const SDL_Rect& a = rect;
    return !(a.x + a.w/2 < b.x - b.w/2 ||
             a.x - a.w/2 > b.x + b.w/2 ||
             a.y + a.h/2 < b.y - b.h/2 ||
             a.y - a.h/2 > b.y + b.h/2);
}

Vec Rectangle::getsize() const
{
	return Vec(rect.w, rect.h);
}

std::vector<Vec> Rectangle::getvertices() const
{
	std::vector<Vec> verts;
	verts.resize(4);
	verts[0] = {float(rect.x), float(rect.y)};
	verts[1] = {float(rect.x+rect.w), float(rect.y)};
	verts[2] = {float(rect.x+rect.w), float(rect.y+rect.h)};
	verts[3] = {float(rect.x), float(rect.y+rect.h)};
	return verts;
}

bool Rectangle::intersectsrotrect(const RotatedRectangle& shape) const
{
	std::array<float, 4> leftrighttopbottom{
               float(rect.x), 
               float(rect.x+rect.w), 
               float(rect.y), 
               float(rect.y+rect.h)
	};

	if(!Intersection::checkprojectionofverticesonrect(shape.getvertices(), leftrighttopbottom))
		return false;
	if(!Intersection::checkprojectionofverticesonrotrect(getvertices(), shape))
		return false;
	return true;
}

bool Rectangle::intersectsannularsector(const AnnularSector& shape) const
{
	return shape.intersectsrect(*this);
}

bool RotatedRectangle::intersects(const Shape& shape) const
{
	return shape.intersectsrotrect(*this);
}

bool RotatedRectangle::intersectsrect(const Rectangle& shape) const
{
	return shape.intersectsrotrect(*this);
}

bool RotatedRectangle::intersectsrotrect(const RotatedRectangle& shape) const
{
	if(!Intersection::checkprojectionofverticesonrotrect(shape.getvertices(), *this))
		return false;
	if(!Intersection::checkprojectionofverticesonrotrect(getvertices(), shape))
		return false;
	return true;
}

bool RotatedRectangle::intersectsannularsector(const AnnularSector& shape) const
{
	return shape.intersectsrotrect(*this);
}

RotatedRectangle::RotatedRectangle(float x_, float y_, int w_, int h_) : RotatedRectangle(x_, y_, w_, h_, Angle::zero)
{}

RotatedRectangle::RotatedRectangle(Vec mid, int w_, int h_, Angle rotation) : RotatedRectangle(mid.x, mid.y, w_, h_, rotation)
{}

RotatedRectangle::RotatedRectangle(float x_, float y_, int w_, int h_, Angle rotation) : 
	mid_x(x_), mid_y(y_), w(w_), h(h_), angle(rotation)
{}

void RotatedRectangle::renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed) const
{
	std::vector<Vec> vertvecs = getvertices();
	std::vector<int> indices = {0, 1, 2, 0, 2, 3};
	r->renderfilledpolygon(vertvecs, indices, color, ported, zoomed);
}

Vec RotatedRectangle::mid() const
{
	return {mid_x,mid_y};
}

bool RotatedRectangle::contains(Vec pos) const
{
	Localvec diff = localcoords(pos, angle, mid());
	if(diff.x>=-w*0.5 && diff.x<=w*0.5 && diff.y>=-h*0.5 && diff.y<=h*0.5)
		return true;
	return false;
}

std::vector<Vec> RotatedRectangle::getvertices() const
{
	std::vector<Vec> verts;
	verts.resize(4);
	const float w2 = w*0.5;
	const float h2 = h*0.5;
	const float c = cos(-angle);
	const float s = sin(-angle);
	verts[0] = {mid_x - w2*c - h2*s, mid_y + h2*c - w2*s};
	verts[1] = {mid_x + w2*c - h2*s, mid_y + h2*c + w2*s};
	verts[2] = {mid_x + w2*c + h2*s, mid_y - h2*c + w2*s};
	verts[3] = {mid_x - w2*c + h2*s, mid_y - h2*c - w2*s};
	return verts;
}

Vec RotatedRectangle::getsize() const
{
	return Vec(w, h);
}

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
	Localvec difflocal = localcoords(point, rightlimitangle, midpoint);
	Angle angletomidpoint(atan2(difflocal.y, difflocal.x));
	if(angletomidpoint.isbetween(Angle::zero, angle))
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

uint8_t discretizecurve(Angle& angle, float radius)
{
	return fmax(1, round(angle.getdegrees()/180*32*radius/100)); // 180 degrees yields 32 segments at radius 100
}

namespace Intersection{

bool checkprojectionofverticesonrotrect(const std::vector<Vec>& verts, const RotatedRectangle& shape)
{
	std::vector<Localvec> rotatedverts;
	rotatedverts.reserve(verts.size());
	for(int i=0; i<verts.size(); i++){
		rotatedverts.push_back(localcoords(verts[i], shape.getorientation(), shape.mid()));
	}
	Vec rotrectsize = shape.getsize();
	float whalf = rotrectsize.x * 0.5;
	float hhalf = rotrectsize.y * 0.5;
	// x
	bool alltotheright = true;
	for(auto& vert : rotatedverts){
		alltotheright &= vert.x > whalf;
	}
	if(alltotheright) return false;

	bool alltotheleft = true;
	for(auto& vert : rotatedverts){
		alltotheleft &= vert.x < -whalf;
	}
	if(alltotheleft) return false;
	// y
	bool allabove = true;
	for(auto& vert : rotatedverts){
		allabove &= vert.y > hhalf;
	}
	if(allabove) return false;

	bool allbelow = true;
	for(auto& vert : rotatedverts){
		allbelow &= vert.y < -hhalf;
	}
	if(allbelow) return false;
	return true;
}

bool checkprojectionofverticesonrect(const std::vector<Vec>& verts, const std::array<float, 4>& lrtb)
{
	// lrtb: left-right-top-bottom values
	// x
	bool alltotheright = true;
	bool alltotheleft = true;
	for(auto& vert : verts){
		alltotheleft &= vert.x < lrtb[0];
		alltotheright &= vert.x > lrtb[1];
	}
	if(alltotheright || alltotheleft) return false;
	// y
	bool allabove = true;
	bool allbelow = true;
	for(auto& vert : verts){
		allabove &= vert.y < lrtb[2];
		allbelow &= vert.y > lrtb[3];
	}
	if(allabove || allbelow) return false;
	return true;
}

bool edgesintersect(const Edge& edge1, const Edge& edge2)
{
	// express edge2 in local coordinates of edge1
	Vec d1 = edge1.endpoint2 - edge1.endpoint1;
	Angle angle1(atan2(-d1.y, d1.x));
	Localvec localendpoint1 = localcoords(edge2.endpoint1, angle1, edge1.endpoint1);
	Localvec localendpoint2 = localcoords(edge2.endpoint2, angle1, edge1.endpoint1);
	// in this system, the vertical component of edge2 must change sign, or there is no collision
	// TODO: This could be optimized, there is no need to compute the local x coords
	if(localendpoint1.y*localendpoint2.y >= 0)
		return false;

	// now do the same test for edge1

	// express edge1 in local coordinates of edge2
	Vec d2 = edge2.endpoint2 - edge2.endpoint1;
	Angle angle2(atan2(-d2.y, d2.x));
	localendpoint1 = localcoords(edge1.endpoint1, angle2, edge2.endpoint1);
	localendpoint2 = localcoords(edge1.endpoint2, angle2, edge2.endpoint1);
	// in this system, the vertical component of edge1 must change sign, or there is no collision
	if(localendpoint1.y*localendpoint2.y >= 0)
		return false;

	return true;
}

bool edgeintersectsarc(const Edge& edge, const Arc& arc)
{
	float r2 = arc.radius * arc.radius;
	float p1tocenter_dist2 = normsquared(edge.endpoint1 - arc.center);
	float p2tocenter_dist2 = normsquared(edge.endpoint2 - arc.center);
	if(p1tocenter_dist2<r2 && p2tocenter_dist2<r2)
		return false; // the edge is enclosed by the circle, no collision
	float xhat = edge.endpoint2.x - edge.endpoint1.x;
	float yhat = -(edge.endpoint2.y - edge.endpoint1.y);
	std::vector<int> discriminantsignstocheck;
	if(p1tocenter_dist2>r2 && p2tocenter_dist2>r2){
		// Both endpoints are outside the circle, check nearest point on edge
		float p1top2_dist2 = normsquared(edge.endpoint1 - edge.endpoint2);
		float mindist2fromedgetocircle = p1tocenter_dist2 - 
			pow(p1tocenter_dist2 + p1top2_dist2 - p2tocenter_dist2, 2) * 0.25 / p1top2_dist2;
		if(mindist2fromedgetocircle>r2){
			return false; // the entire line is outside the circle
		}
		xhat = edge.endpoint2.x - edge.endpoint1.x;
		yhat = -(edge.endpoint2.y - edge.endpoint1.y);
		Angle edgeangle(atan2(yhat, xhat)); // TODO: Turn this into a constructor/function here and everywhere else in this file using atan2
		Localvec clocal = localcoords(arc.center, edgeangle, edge.endpoint1);
		if(clocal.x<0 || clocal.x>norm(edge.endpoint2-edge.endpoint1))
			return false; // edge does not intersect circle
		// Edge intersects circle, we must check both intersections vs the arc angles
		discriminantsignstocheck.push_back(-1);
		discriminantsignstocheck.push_back(1);
	}
	else{
		xhat = edge.endpoint2.x - edge.endpoint1.x;
		yhat = -(edge.endpoint2.y - edge.endpoint1.y);
		// The edge pierces through the circle
		// There will be two solutions but we must only check one of them vs the arc angles
		bool pointingoutwards = (p2tocenter_dist2>p1tocenter_dist2);
		if(pointingoutwards) discriminantsignstocheck.push_back(1);
		else discriminantsignstocheck.push_back(-1);
	}

	// Compute line intersection angles and compare to arc limit angles
	float mixedterm = -(edge.endpoint1.y-arc.center.y)*xhat - (edge.endpoint1.x-arc.center.x)*yhat;
	float discriminant = sqrt((xhat*xhat+yhat*yhat)*r2 - mixedterm*mixedterm);
	for(int discsign : discriminantsignstocheck){
		float nom = xhat*mixedterm + yhat*discsign*discriminant;
		float denom = -yhat*mixedterm + xhat*discsign*discriminant;
		Angle intersectionangle(atan2(nom, denom));

		if(intersectionangle.isbetween(arc.rightangle, arc.rightangle+arc.angle))
			return true;
	}
	return false;
}

bool anyedgesintersect(const std::vector<Edge>& edges1, const std::vector<Edge>& edges2){
	for(auto& edge: edges1){
		for(auto& otheredge : edges2){
			if(Intersection::edgesintersect(edge, otheredge))
				return true;
		}
	}
	return false;
}

} // namespace Intersection