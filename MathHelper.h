#pragma once
#include <emmintrin.h>
#include <random>

#define MY_RAND_MAX 25000000


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



}

