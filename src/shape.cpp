#include<iostream>
#include<string>
#include "bahnhof/common/shape.h"
#include "bahnhof/graphics/rendering.h"

RotatedRectangle::RotatedRectangle(float x_, float y_, int w_, int h_) : RotatedRectangle(x_, y_, w_, h_, 0)
{}

RotatedRectangle::RotatedRectangle(float x_, float y_, int w_, int h_, float topleftrotation) : 
	x(x_), y(y_), w(w_), h(h_), angle(topleftrotation)
{}

RotatedRectangle::RotatedRectangle(SDL_Rect& rect) : RotatedRectangle(rect, 0)
{}

RotatedRectangle::RotatedRectangle(SDL_Rect& rect, float topleftrotation) : 
	x(rect.x), y(rect.y), w(rect.w), h(rect.h), angle(topleftrotation)
{}

void RotatedRectangle::renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed)
{
	SDL_Vertex verts[4];
	const float w2 = w*0.5;
	const float h2 = h*0.5;
	const float c = cos(-angle);
	const float s = sin(-angle);
	verts[0].position = {x - w2*c - h2*s, y + h2*c - w2*s};
	verts[1].position = {x + w2*c - h2*s, y + h2*c + w2*s};
	verts[2].position = {x + w2*c + h2*s, y - h2*c + w2*s};
	verts[3].position = {x - w2*c + h2*s, y - h2*c - w2*s};
	int indices[6] = {0, 1, 2, 0, 2, 3};
	r->renderfilledpolygon(verts, 4, indices, 6, color, ported, zoomed);
}

Vec RotatedRectangle::mid()
{
	return {x,y};
}
