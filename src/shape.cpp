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
	verts[2] = {float(rect.x), float(rect.y+rect.h)};
	verts[3] = {float(rect.x+rect.w), float(rect.y+rect.h)};
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

RotatedRectangle::RotatedRectangle(float x_, float y_, int w_, int h_) : RotatedRectangle(x_, y_, w_, h_, 0)
{}

RotatedRectangle::RotatedRectangle(Vec mid, int w_, int h_, float rotation) : RotatedRectangle(mid.x, mid.y, w_, h_, rotation)
{}

RotatedRectangle::RotatedRectangle(float x_, float y_, int w_, int h_, float rotation) : 
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
	Vec diff = localcoords(pos, angle, mid());
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

AnnularSector::AnnularSector(Vec frompos, float fromdir, Vec topos, float thickness)
{
	Vec d = localcoords(topos, fromdir, frompos);
	float dx = d.x; float dy = -d.y;
	float radius = 0.5*(dy*dy+dx*dx)/dy;
	angle = abs(atan2(dx, sign(dy)*(radius-dy)));
	midpoint = globalcoords({0,-radius}, fromdir, frompos);
	angle = truncate(abs(angle), 2*pi);
	std::cout<<angle*180/pi<<std::endl;
	innerradius = abs(radius) - 0.5*thickness;
	outerradius = abs(radius) + 0.5*thickness;
	if(dy<0)
		rightlimitangle = fromdir - pi/2;
	else
		rightlimitangle = fromdir - angle + pi/2;
	rightlimitangle = truncate(rightlimitangle, 2*pi);
}

void AnnularSector::renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed) const
{
	std::vector<Vec> vertvecs = getvertices();
	int nSegments = (vertvecs.size()-2)/2;
	std::vector<int> indices;
	indices.reserve(2*3*nSegments);
	for(int i=0; i<nSegments*2; i++){
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i+2);
	}
	r->renderfilledpolygon(vertvecs, indices, color, ported, zoomed);
	r->renderfilledrectangle({int(round(midpoint.x))-3,int(round(midpoint.y))-3,6,6});
}

Vec AnnularSector::mid() const
{
	return midpoint;
}

float AnnularSector::getorientation() const
{
	return rightlimitangle + angle * 0.5;
}

std::vector<Vec> AnnularSector::getvertices() const
{
	int nSegments = 20;
	std::vector<Vec> verts;
	verts.reserve(nSegments*2+2);
	float mid_x = midpoint.x;
	float mid_y = midpoint.y;
	float alpha = -rightlimitangle;
	float angleneg = -angle;
	for(int i=0; i<nSegments+1; i++){
		verts.emplace_back(Vec(mid_x + innerradius*cos(alpha), mid_y + innerradius*sin(alpha)));
		verts.emplace_back(Vec(mid_x + outerradius*cos(alpha), mid_y + outerradius*sin(alpha)));
		alpha += angleneg/nSegments;
	}
	return verts;
}

bool AnnularSector::contains(Vec point) const
{
	Vec diff = point-midpoint;
	float distance = norm(diff);
	if(distance>outerradius) return false;
	if(distance<innerradius) return false;
	Vec difflocal = localcoords(point, rightlimitangle, midpoint);
	float angletomidpoint = atan2(difflocal.y, difflocal.x);
	angletomidpoint = truncate(angletomidpoint, 2*pi);
	if(angletomidpoint<0) return false;
	if(angletomidpoint>angle) return false;
	return true;
}

bool AnnularSector::intersects(const Shape& shape) const
{
	return shape.intersectsannularsector(*this);
}

bool AnnularSector::intersectsrect(const Rectangle&) const
{
	return false;
}

bool AnnularSector::intersectsrotrect(const RotatedRectangle&) const
{
	return false;
}

bool AnnularSector::intersectsannularsector(const AnnularSector&) const
{
	return false; // for now this won't happen
}

namespace Intersection{

bool checkprojectionofverticesonrotrect(const std::vector<Vec>& verts, const RotatedRectangle& shape)
{
	std::vector<Vec> rotatedverts(verts.size());
	for(int i=0; i<verts.size(); i++){
		rotatedverts[i] = localcoords(verts[i], shape.getorientation(), shape.mid());
	}
	Vec rotrectsize = shape.getsize();
	float whalf = rotrectsize.x * 0.5;
	float hhalf = rotrectsize.y * 0.5;
	std::array<float, 4> leftrighttopbottom = {
		float(-whalf), 
		float(whalf), 
		float(-hhalf), 
		float(hhalf)
	};
	return Intersection::checkprojectionofverticesonrect(rotatedverts, leftrighttopbottom);
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

} // namespace Intersection