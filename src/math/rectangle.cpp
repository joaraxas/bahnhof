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
