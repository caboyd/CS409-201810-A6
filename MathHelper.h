#pragma once
#include <emmintrin.h>
#include "lib/ObjLibrary/Vector2.h"
#include "lib/ObjLibrary/Vector3.h"
using ObjLibrary::Vector2;
using ObjLibrary::Vector3;
namespace MathHelper
{

	constexpr double M_PI = 3.14159265358979323846;
	constexpr double M_PI_2 = 1.57079632679489661923;
	constexpr double M_2PI = 6.28318530718;

	constexpr double M_SQRT2 = 1.41421356237309504880;
	constexpr double M_SQRT2_2 = 0.707106781186548;


	//Perform square root using intel intrisics for performance
	inline float squareRoot(const float x)
	{
		const float result = _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
		return(result);
	}

	//calcuate distance between two 2d points
	inline float distancef(const float x1, const float y1, const float x2, const float y2)
	{
		const float diff_x = x1 - x2;
		const float diff_y = y1 - y2;
		const float x = (diff_x * diff_x + diff_y * diff_y);
		const float result = squareRoot(x);
		return result;
	}

	// Compute barycentric coordinates (u, v, w) for
	// point p with respect to triangle (a, b, c)
	inline void barycentric(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c, float &u, float &v, float &w)
	{
		Vector2 v0 = b - a, v1 = c - a, v2 = p - a;
		const double d00 = v0.dotProduct(v0);
		const double d01 = v0.dotProduct(v1);
		const double d11 = v1.dotProduct(v1);
		const double d20 = v2.dotProduct(v0);
		const double d21 = v2.dotProduct(v1);
		const double inv_denom = 1 / (d00 * d11 - d01 * d01);
		v = float((d11 * d20 - d01 * d21) * inv_denom);
		w = float((d00 * d21 - d01 * d20) * inv_denom);
		u = float(1.0f - v - w);
	}

	inline Vector3 truncate(const Vector3& v, const float max)
	{
		if(v.getNorm() <= max)
		{
			return v;
		}else
		{
			return v.getNormalized() * max;
		}
	}

	inline Vector3 minVector(const Vector3& v, const float min)
	{
		if(v.getNorm() < min)
		{
			return v.getNormalized() * min;
		}else
		{
			return v;
		}
	}
}

