#pragma once
#include <emmintrin.h>
#include <random>
#include "lib/ObjLibrary/Vector2.h"

#define MY_RAND_MAX 25000000

using ObjLibrary::Vector2;

namespace MathHelper
{

	const double M_PI = 3.14159265358979323846;
	const double M_PI_2 = 1.57079632679489661923;
	const double M_2PI = 6.28318530718;

	const double M_SQRT2 = 1.41421356237309504880;
	const double M_SQRT2_2 = 0.707106781186548;


	//Perform square root using intel intrisics for performance
	inline float squareRoot(float x)
	{
		const float result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
		return(result);
	}

	//calcuate distance between two 2d points
	inline float distancef(float x1, float y1, float x2, float y2)
	{
		float diff_x = x1 - x2;
		float diff_y = y1 - y2;
		float x = (diff_x * diff_x + diff_y * diff_y);
		float result = squareRoot(x);
		return result;
	}

	// Compute barycentric coordinates (u, v, w) for
	// point p with respect to triangle (a, b, c)
	inline void Barycentric(Vector2 p, Vector2 a, Vector2 b, Vector2 c, float &u, float &v, float &w)
	{
		Vector2 v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = v0.dotProduct(v0);
		float d01 = v0.dotProduct(v1);
		float d11 = v1.dotProduct(v1);
		float d20 = v2.dotProduct(v0);
		float d21 = v2.dotProduct(v1);
		float inv_denom = 1 / (d00 * d11 - d01 * d01);
		v = (d11 * d20 - d01 * d21) * inv_denom;
		w = (d00 * d21 - d01 * d20) * inv_denom;
		u = 1.0f - v - w;
	}
}

