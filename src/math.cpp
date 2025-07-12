#include<iostream>
#include<string>
#include "bahnhof/common/math.h"
#include "bahnhof/graphics/rendering.h"

Vec::Vec()
{
	x = 0;
	y = 0;
}

Vec::Vec(float xstart, float ystart)
{
	x = xstart;
	y = ystart;
}

Vec Vec::operator-(Vec v)
{
	Vec out(0,0);
	out.x = x-v.x;
	out.y = y-v.y;
	return out;
}

Vec Vec::operator+(Vec v)
{
	Vec out(0,0);
	out.x = x+v.x;
	out.y = y+v.y;
	return out;
}

Vec Vec::operator*(float a)
{
	Vec out(0,0);
	out.x = x*a;
	out.y = y*a;
	return out;
}

Vec Vec::operator/(float a)
{
	Vec out(0,0);
	out.x = x/a;
	out.y = y/a;
	return out;
}

float norm(Vec v)
{
	return std::sqrt(v.x*v.x + v.y*v.y);
}

Vec localcoords(Vec globalvec, float angle, Vec origin)
{
	// pass from left-hand system to right-hand and rotate by -angle
	Vec localvec;
	localvec.x = std::cos(angle)*(globalvec.x - origin.x) + std::sin(angle)*-(globalvec.y - origin.y);
	localvec.y =-std::sin(angle)*(globalvec.x - origin.x) + std::cos(angle)*-(globalvec.y - origin.y);
	return localvec;
}

Vec globalcoords(Vec localvec, float angle, Vec origin)
{
	// pass from right-hand system to left-hand and rotate by angle
	Vec globalvec;
	globalvec.x = std::cos(angle)*(localvec.x) - std::sin(angle)*(localvec.y);
	globalvec.y = std::sin(angle)*(localvec.x) + std::cos(angle)*(localvec.y);
	globalvec.y = -globalvec.y;
	globalvec = globalvec + origin;
	return globalvec;
}

Shape::Shape(float x_, float y_, int w_, int h_) : Shape(x_, y_, w_, h_, 0)
{}

Shape::Shape(float x_, float y_, int w_, int h_, float topleftrotation) : x(x_), y(y_), w(w_), h(h_), angle(topleftrotation) 
{}

Shape::Shape(SDL_Rect& rect) : Shape(rect, 0)
{}

Shape::Shape(SDL_Rect& rect, float topleftrotation) : x(rect.x), y(rect.y), w(rect.w), h(rect.h), angle(topleftrotation)
{}

void Shape::renderfilled(Rendering* r, SDL_Color color, bool ported, bool zoomed)
{
	SDL_Vertex verts[4];
	verts[0].position = {x, y};
	verts[1].position = {x+w, y};
	verts[2].position = {x+w, y+h};
	verts[3].position = {x, y+h};
	int indices[6] = {0, 1, 2, 0, 2, 3};
	r->renderfilledpolygon(verts, 4, indices, 6, color, ported, zoomed);
}

Vec Shape::mid()
{
	return {x,y};
}

float sign(float a)
{
	if(a<0)
		return -1;
	return 1;
}

float truncate(float dir)
{
	return dir - pi*std::floor(dir/pi);
}

int randint(int maxinclusive)
{
	return rand() % (maxinclusive+1);
}

