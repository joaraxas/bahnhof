#include<iostream>
#include<string>
#include "bahnhof/common/math.h"

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
	return sqrt(v.x*v.x + v.y*v.y);
}

Vec localcoords(Vec globalvec, float angle, Vec origin)
{
	// pass from left-hand system to right-hand and rotate by -angle
	Vec localvec;
	localvec.x = cos(angle)*(globalvec.x - origin.x) + sin(angle)*-(globalvec.y - origin.y);
	localvec.y =-sin(angle)*(globalvec.x - origin.x) + cos(angle)*-(globalvec.y - origin.y);
	return localvec;
}

Vec globalcoords(Vec localvec, float angle, Vec origin)
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

float truncate(float dir)
{
	return dir - pi*floor(dir/pi);
}

int randint(int maxinclusive)
{
	return rand() % (maxinclusive+1);
}

