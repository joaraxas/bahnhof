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

Angle Vec::toangle() const
{
	if(x==0 && y==0) {std::cout<<"warning: returning zero from zero vector in Vec::toangle()"<<std::endl; return Angle::zero;}
	return Angle(atan2(-y, x));
}

Localvec operator+(Localvec v1, const Localvec& v2) {return v1+=v2;}
Localvec operator-(Localvec v1, const Localvec& v2) {return v1-=v2;}

std::ostream& operator<<(std::ostream& os, const Vec& v)
{
	os << '[' << v.x << ', ' << v.y << ']';
	return os;
}

std::ostream& operator<<(std::ostream& os, const Localvec& v)
{
	os << '(' << v.x << ', ' << v.y << ')';
	return os;
}

Localvec localcoords(const Vec& globalvec, const Angle& angle, const Vec& origin)
{
	const Localvec localvec{globalvec.x - origin.x, -(globalvec.y - origin.y)};
	if(angle == Angle::zero)
		return localvec;
	// pass from left-hand system to right-hand and rotate by -angle
	const float sa = sin(angle);
	const float ca = cos(angle);
	return Localvec{
		ca*localvec.x + sa*localvec.y,
	   -sa*localvec.x + ca*localvec.y
	};
}

Vec globalcoords(const Localvec& localvec, const Angle& angle, const Vec& origin)
{
	if(angle == Angle::zero)
		return Vec{origin.x + localvec.x, origin.y - localvec.y};
	// pass from right-hand system to left-hand and rotate by angle
	Vec globalvec = origin;
	float sa = sin(angle);
	float ca = cos(angle);
	if(localvec.x!=0){
		globalvec.x += ca*(localvec.x);
		globalvec.y -= sa*(localvec.x);
	}
	if(localvec.y!=0){
		globalvec.x -= sa*(localvec.y);
		globalvec.y -= ca*(localvec.y);
	}
	return globalvec;
}

Angle Localvec::toangle() const
{
	if(x==0 && y==0) {std::cout<<"warning: returning zero from zero vector in Localvec::toangle()"<<std::endl; return Angle::zero;}
	return Angle(atan2(y, x));
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

