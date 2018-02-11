#pragma once
#include "MathHelper.h"

//Determines if there is an intersetion between a circle of radius r1 at
//	position x1,y2 and a circle with radius r2 at position x2,r2
inline bool circleIntersection(const float x1, const float y1, const float r1, const float x2, const float y2, const float r2)
{
	const float dx = x1 - x2;
	const float dy = y1 - y2;

	const bool result = (dx*dx + dy*dy) <= (r1 + r2) * (r1 + r2);
	return result;
}

inline bool circleIntersection(const Vector2& p1, const float r1, const Vector2& p2, const float r2)
{
	return circleIntersection(float(p1.x),float(p1.y), r1, float(p2.x), float(p2.y), r2);
}

//Determines if there is an intersetion between a circle of radius r1 at
//	position x1,y2 and a circle with radius r2 at position x2,r2
inline bool pointCircleIntersection(const float x1, const float y1, const float x2, const float y2, const float r2)
{
	return circleIntersection(x1, y1, 0, x2, y2, r2);
}
inline bool pointCircleIntersection(const Vector2& p1, const Vector2& p2, const float r2)
{
	return circleIntersection(float(p1.x),float(p1.y), 0, float(p2.x), float(p2.y), r2);
}

//Determines if there is an intersection between two cylinders of radius r1 at position p1 with a height of h1
//against a cylinder of radius r2 at position p2 with a height of h2
inline bool cylinderIntersection(const Vector3& p1, float r1, float half_heigth1, const Vector3& p2, float r2, float half_height2)
{
	const float h1 = half_heigth1;
	const float h2 = half_height2;
	//Check for height collision first
	//Top of p1 doesnt reach bottom of p2
	//Or bottom of p1 doesnt reach  top of p2
	if(p1.y + h1 < p2.y - h2 || p1.y - h1 > p2.y + h2)
		return false;

	//Check for circle collision second
	return circleIntersection(float(p1.x), float(p1.z), r1,float(p2.x), float(p2.z), r2);

}