#pragma once
#include "MathHelper.h"

//Determines if there is an intersetion between a circle of radius r1 at
//	position x1,y2 and a circle with radius r2 at position x2,r2
inline bool circleIntersection(const float x1, const float y1, const float r1, const float x2, const float y2, const float r2)
{
	const float dx = x1 - x2;
	const float dy = y1 - y2;

	const float result = (dx*dx + dy*dy) <= (r1 + r2) * (r1 + r2);
	return result;
}