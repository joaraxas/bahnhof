#include "bahnhof/common/shape.h"
#include "bahnhof/common/geometry.h"
#include "bahnhof/graphics/rendering.h"

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

bool RotatedRectangle::intersectsringsector(const Ringsector& shape) const
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
