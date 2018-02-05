#pragma once
#include <emmintrin.h>

//SIMD functions to perform many calculations more quickly

struct alignas(16) sse_t
{
	float sse_data[4];
};

struct Vec3SOA
{
	sse_t* x;
	sse_t* y;
	sse_t* z;
};


inline void addAndSubSIMD(float* result_add, float* result_sub, const int size, const float* x1, const float* x2)
{
	assert(size % 4 == 0);

	for (int i = 0; i < size / 4; i++)
	{
		const int k = 4 * i;
		_mm_store_ps(result_sub + k, _mm_sub_ps(_mm_load_ps(x1 + k), _mm_load_ps(x2 + k)));
		_mm_store_ps(result_add + k, _mm_add_ps(_mm_load_ps(x1 + k), _mm_load_ps(x2 + k)));
	}
};

inline void addSIMD(float* result_add, const int size, const float* x1, const float* x2)
{
	assert(size % 4 == 0);

	for (int i = 0; i < size / 4; i++)
	{
		const int k = 4 * i;
		_mm_store_ps(result_add + k, _mm_add_ps(_mm_load_ps(x1 + k), _mm_load_ps(x2 + k)));
	}
};

inline void subSIMD( float* result_sub, const int size, const float* x1, const float* x2)
{
	assert(size % 4 == 0);

	for (int i = 0; i < size / 4; i++)
	{
		const int k = 4 * i;
		_mm_store_ps(result_sub + k, _mm_sub_ps(_mm_load_ps(x1 + k), _mm_load_ps(x2 + k)));
	}
};

inline void addAndSub1ToManySIMD(float* result_add, float* result_sub, const int size, const float x1, const float* x2)
{
	assert(size % 4 == 0);

	const __m128 x = _mm_set_ps1(x1);
	for (int i = 0; i < size / 4; i++)
	{
		const int k = 4 * i;
		_mm_store_ps(result_sub + k, _mm_sub_ps(x, _mm_load_ps(x2 + k)));
	}
	for (int i = 0; i < size / 4; i++)
	{
		const int k = 4 * i;
		_mm_store_ps(result_add + k, _mm_add_ps(x, _mm_load_ps(x2 + k)));
	}


};

inline void add1ToManySIMD(float* result_add, const int size, const float x1, const float* x2)
{
	assert(size % 4 == 0);

	const __m128 x = _mm_set_ps1(x1);
	for (int i = 0; i < size / 4; i++)
	{
		const int k = 4 * i;
		_mm_store_ps(result_add + k, _mm_add_ps(x, _mm_load_ps(x2 + k)));
	}


};

inline void sub1ToManySIMD(float* result_sub, const int size, const float x1, const float* x2)
{
	assert(size % 4 == 0);

	const __m128 x = _mm_set_ps1(x1);
	for (int i = 0; i < size / 4; i++)
	{
		const int k = 4 * i;
		_mm_store_ps(result_sub + k, _mm_sub_ps(x, _mm_load_ps(x2 + k)));
	}

};
inline float maxSIMD(const int size, const float *x)
{
	assert(size % 4 == 0);

	__m128 mx = _mm_load_ps(x);
	for (int i = 4; i < size / 4; i++)
	{
		const int k = 4 * i;
		mx = _mm_max_ps(mx, _mm_load_ps(x + k));
	}

	float a[4];
	_mm_store_ps(a, mx);
	return max(max(a[0], a[1]), max(a[2], a[3]));
}

inline float minSIMD(const int size, const float *x)
{
	assert(size % 4 == 0);

	__m128 mx = _mm_load_ps(x);
	for (int i = 4; i < size / 4; i++)
	{
		const int k = 4 * i;
		mx = _mm_min_ps(mx, _mm_load_ps(x + k));
	}

	float a[4];
	_mm_store_ps(a, mx);
	return min(min(a[0], a[1]), min(a[2], a[3]));
}




inline void vec2DistanceSquaredManyToManySIMD(float* result, const int size1, const float* x1, const float* y1,
	const int size2, const float* x2, const float* y2)
{
	assert(size2 % 4 == 0);

	for (int i = 0; i < size1; i++)
	{
		for (int j = 0; j < size2 / 4; j++)
		{
			const int k = i * size2 + 4 * j;
			const __m128 dx = _mm_sub_ps(_mm_load_ps1(x1 + i), _mm_load_ps(x2 + 4 * j));
			const __m128 dy = _mm_sub_ps(_mm_load_ps1(y1 + i), _mm_load_ps(y2 + 4 * j));
			const __m128 x_2 = _mm_mul_ps(dx, dx);
			const __m128 y_2 = _mm_mul_ps(dy, dy);
			_mm_store_ps(result + k, _mm_add_ps(x_2, y_2));

		}
	}
}
//Distance between a 16 byte aligned float array of size1 and a 16 byte aligned float array of size2
//Results are stored in result
inline void vec2DistanceManyToManySIMD(float* result, const int size1, const float* x1, const float* y1,
	const int size2, const float* x2, const float* y2)
{
	assert(size2 % 4 == 0);

	for (int i = 0; i < size1; i++)
	{
		for (int j = 0; j < size2 / 4; j++)
		{
			const int k = i * size2 + 4 * j;
			const __m128 dx = _mm_sub_ps(_mm_load_ps(x1 + i), _mm_load_ps(x2 + 4 * j));
			const __m128 dy = _mm_sub_ps(_mm_load_ps(y1 + i), _mm_load_ps(y2 + 4 * j));
			const __m128 x_2 = _mm_mul_ps(dx, dx);
			const __m128 y_2 = _mm_mul_ps(dy, dy);
			_mm_store_ps(result + k, _mm_sqrt_ps(_mm_add_ps(x_2, y_2)));
		}
	}
}

//Distance between 1 point and a 16 byte aligned float array of size2 points
inline void vec2Distance1ToManySIMD(float* result, const float x1, const float y1,
	const int size2, const float* x2, const float* y2)
{
	assert(size2 % 4 == 0);

	for (int j = 0; j < size2 / 4; j++)
	{
		const int k = 4 * j;
		const __m128 dx = _mm_sub_ps(_mm_set_ps1(x1), _mm_load_ps(x2 + k));
		const __m128 dy = _mm_sub_ps(_mm_set_ps1(y1), _mm_load_ps(y2 + k));
		const __m128 x_2 = _mm_mul_ps(dx, dx);
		const __m128 y_2 = _mm_mul_ps(dy, dy);
		_mm_store_ps(result + k, _mm_sqrt_ps(_mm_add_ps(x_2, y_2)));
	}

}

inline void SquareRootSIMD(float* result, const int size1, const float* x)
{
	for (int i = 0; i < size1; i += 4)
	{
		_mm_store_ps(result + i, _mm_sqrt_ps(_mm_load_ps(x + i)));
	}
}