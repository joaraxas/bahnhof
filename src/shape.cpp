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

bool Rectangle::intersects(const Shape* shape) const
{
	return shape->intersectsrect(this);
}

bool Rectangle::intersectsrect(const Rectangle* shape) const
{
	const SDL_Rect& b = shape->rect;
	const SDL_Rect& a = rect;
    return !(a.x + a.w/2 < b.x - b.w/2 ||
             a.x - a.w/2 > b.x + b.w/2 ||
             a.y + a.h/2 < b.y - b.h/2 ||
             a.y - a.h/2 > b.y + b.h/2);
}

// bool checkprojectionofverticesonrect(std::array<Vec, 4> verts, std::array<float, 4> rltb)
// {
// 	// x
// 	bool alltotheright = true;
// 	bool alltotheleft = true;
// 	for(auto& vert : verts){
// 		alltotheright &= vert.x > rltb;
// 		alltotheleft &= vert.x < left;
// 	}
// 	if(alltotheright || alltotheleft) return false;
// 	// y
// 	int top = rect.y;
// 	int bottom = rect.y+rect.h;
// 	bool allabove = true;
// 	bool allbelow = true;
// 	for(auto& vert : verts){
// 		allabove &= vert.y > rect.y+rect.h;
// 		allbelow &= vert.y < rect.y;
// 	}
// 	if(allabove || allbelow) return false;
// }

bool Rectangle::intersectsrotrect(const RotatedRectangle* shape) const
{
	std::array<Vec, 4> verts = shape->getvertices();
	int left = rect.x;
	int right = rect.x+rect.w;
	// x
	bool alltotheright = true;
	bool alltotheleft = true;
	for(auto& vert : verts){
		alltotheright &= vert.x > right;
		alltotheleft &= vert.x < left;
	}
	if(alltotheright || alltotheleft) return false;
	// y
	int top = rect.y;
	int bottom = rect.y+rect.h;
	bool allabove = true;
	bool allbelow = true;
	for(auto& vert : verts){
		allabove &= vert.y > rect.y+rect.h;
		allbelow &= vert.y < rect.y;
	}
	if(allabove || allbelow) return false;
	return true;
}

bool RotatedRectangle::intersects(const Shape* shape) const
{
	return shape->intersectsrotrect(this);
}

bool RotatedRectangle::intersectsrect(const Rectangle* shape) const
{
	return shape->intersectsrotrect(this);
}

bool RotatedRectangle::intersectsrotrect(const RotatedRectangle* shape) const
{
	localcoords(shape->mid(), angle, mid());
	return false;
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
	SDL_Vertex verts[4];
	std::array<Vec, 4> vertvecs = getvertices();
	for(int i=0; i<4; i++){
		verts[i].position = {vertvecs[i].x, vertvecs[i].y};
	}
	int indices[6] = {0, 1, 2, 0, 2, 3};
	r->renderfilledpolygon(verts, 4, indices, 6, color, ported, zoomed);
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

std::array<Vec, 4> RotatedRectangle::getvertices() const
{
	std::array<Vec, 4> verts;
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