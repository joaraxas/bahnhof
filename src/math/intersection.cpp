#include "bahnhof/common/shape.h"

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
	Angle angle1(edge1.endpoint2 - edge1.endpoint1);
	Localvec localendpoint1 = localcoords(edge2.endpoint1, angle1, edge1.endpoint1);
	Localvec localendpoint2 = localcoords(edge2.endpoint2, angle1, edge1.endpoint1);
	// in this system, the vertical component of edge2 must change sign, or there is no collision
	// TODO: This could be optimized, there is no need to compute the local x coords
	if(localendpoint1.y*localendpoint2.y >= 0)
		return false;

	// now do the same test for edge1

	// express edge1 in local coordinates of edge2
	Angle angle2(edge2.endpoint2 - edge2.endpoint1);
	localendpoint1 = localcoords(edge1.endpoint1, angle2, edge2.endpoint1);
	localendpoint2 = localcoords(edge1.endpoint2, angle2, edge2.endpoint1);
	// in this system, the vertical component of edge1 must change sign, or there is no collision
	if(localendpoint1.y*localendpoint2.y >= 0)
		return false;

	return true;
}

bool edgeintersectsarc(const Edge& edge, const Arc& arc)
{
	float rsq = arc.radius * arc.radius;
	Localvec ctop1 = localcoords(edge.endpoint1, Angle::zero, arc.center);
	Localvec ctop2 = localcoords(edge.endpoint2, Angle::zero, arc.center);
	float p1tocenter_dist2 = normsquared(ctop1);
	float p2tocenter_dist2 = normsquared(ctop2);
	if(p1tocenter_dist2<rsq && p2tocenter_dist2<rsq)
		return false; // the edge is enclosed by the circle, no collision
	Localvec p1top2 = localcoords(edge.endpoint2, Angle::zero, edge.endpoint1);
	std::vector<int> discriminantsignstocheck;
	if(p1tocenter_dist2>rsq && p2tocenter_dist2>rsq){
		// Both endpoints are outside the circle, check nearest point on edge
		float p1top2_dist2 = normsquared(p1top2);
		float mindist2fromedgetocircle = p1tocenter_dist2 - 
			pow(p1tocenter_dist2 + p1top2_dist2 - p2tocenter_dist2, 2) * 0.25 / p1top2_dist2;
		if(mindist2fromedgetocircle>rsq){
			return false; // the entire line is outside the circle
		}
		Angle edgeangle(p1top2);
		Localvec clocal = localcoords(arc.center, edgeangle, edge.endpoint1);
		if(clocal.x<0 || clocal.x>sqrt(p1top2_dist2))
			return false; // edge does not intersect circle
		// Edge intersects circle, we must check both intersections vs the arc angles
		discriminantsignstocheck.push_back(-1);
		discriminantsignstocheck.push_back(1);
	}
	else{
		// The edge pierces through the circle
		// There will be two solutions but we must only check one of them vs the arc angles
		bool pointingoutwards = (p2tocenter_dist2>p1tocenter_dist2);
		if(pointingoutwards) discriminantsignstocheck.push_back(1);
		else discriminantsignstocheck.push_back(-1);
	}

	// Compute line intersection angles and compare to arc limit angles
	float mixedterm = ctop1.y*p1top2.x - ctop1.x*p1top2.y;
	float discriminant = sqrt(normsquared(p1top2)*rsq - mixedterm*mixedterm);
	for(int discsign : discriminantsignstocheck){
		float nom = p1top2.x*mixedterm + p1top2.y*discsign*discriminant;
		float denom = -p1top2.y*mixedterm + p1top2.x*discsign*discriminant;
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