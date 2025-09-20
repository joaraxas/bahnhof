#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<iostream>
#include<string>
#include "bahnhof/common/math.h"
#include "bahnhof/common/orientation.h"

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

Vec::Vec(const SDL_FPoint& point)
{
	x = point.x;
	y = point.y;
}

Vec Vec::operator-(const Vec& v) const
{
	Vec out(0,0);
	out.x = x-v.x;
	out.y = y-v.y;
	return out;
}

Vec Vec::operator+(const Vec& v) const
{
	Vec out(0,0);
	out.x = x+v.x;
	out.y = y+v.y;
	return out;
}

Vec Vec::operator*(float a) const
{
	Vec out(0,0);
	out.x = x*a;
	out.y = y*a;
	return out;
}

Vec operator*(float a, const Vec& v)
{
	return Vec{a*v.x, a*v.y};
}

Vec Vec::operator/(float a) const
{
	Vec out(0,0);
	out.x = x/a;
	out.y = y/a;
	return out;
}

float norm(const Vec& v)
{
	return std::sqrt(normsquared(v));
}

float normsquared(const Vec& v)
{
	return v.x*v.x + v.y*v.y;
}

Localvec::Localvec(const Vec& origin, const Angle& angle, const Vec& globalvec)
{
	x = cos(angle)*(globalvec.x - origin.x) + sin(angle)*-(globalvec.y - origin.y);
	y =-sin(angle)*(globalvec.x - origin.x) + cos(angle)*-(globalvec.y - origin.y);
}

Localvec localcoords(Vec globalvec, Angle orientation, Vec origin)
{
	// pass from left-hand system to right-hand and rotate by -angle
	return Localvec(origin, orientation, globalvec);
}

Vec globalcoords(Localvec localvec, Angle angle, Vec origin)
{
	// pass from right-hand system to left-hand and rotate by angle
	Vec globalvec;
	globalvec.x = cos(angle)*(localvec.x) - sin(angle)*(localvec.y);
	globalvec.y = sin(angle)*(localvec.x) + cos(angle)*(localvec.y);
	globalvec.y = -globalvec.y;
	globalvec = globalvec + origin;
	return globalvec;
}

float sign(float a)
{
	if(a<0)
		return -1;
	return 1;
}

float truncate(float dir, float denominator)
{
	return dir - denominator*std::floor(dir/denominator);
}

float anglediff(float a, float b, float wraparound)
{
	return std::remainder(a - b, wraparound);
}

int randint(int maxinclusive)
{
	return rand() % (maxinclusive+1);
}

