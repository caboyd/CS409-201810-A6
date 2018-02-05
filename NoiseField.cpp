#include "NoiseField.h"
#include <cmath>

NoiseField::NoiseField(float grid_size, float a, unsigned x1, unsigned x2, unsigned y1, unsigned y2, unsigned q0, unsigned q1,
	unsigned q2)
{
	GRID_SIZE = grid_size;
	amplitude = a;
	SEED_X1 = x1;
	SEED_X2 = x2;
	SEED_Y1 = y1;
	SEED_Y2 = y2;
	SEED_Q0 = q0;
	SEED_Q1 = q1;
	SEED_Q2 = q2;
}

float NoiseField::valueNoise(const float x,const  float y) const
{
	// calculate noise here
	const int x0 = int(floor(x / GRID_SIZE));
	const int y0 = int(floor(y / GRID_SIZE));

	const float x_frac = x / GRID_SIZE - x0;
	const float y_frac = y / GRID_SIZE - y0;

	const int x1 = x0 + 1;
	const int y1 = y0 + 1;

	const unsigned value00 = pseudorandom(x0, y0);
	const unsigned value01 = pseudorandom(x0, y1);
	const unsigned value10 = pseudorandom(x1, y0);
	const unsigned value11 = pseudorandom(x1, y1);

	const float x_fade1 = fade(x_frac);
	const float y_fade1 = fade(y_frac);
	const float x_fade0 = 1.0f - x_fade1;
	const float y_fade0 = 1.0f - y_fade1;

	const unsigned value0 = unsigned(value00 * y_fade0) +
		unsigned(value01 * y_fade1);
	const unsigned value1 = unsigned(value10 * y_fade0) +
		unsigned(value11 * y_fade1);
	const unsigned value = unsigned(value0 * x_fade0) +
		unsigned(value1 * x_fade1);

	const float result = ((float(value) / UINT_MAX) * 2.0f - 1.0f) * amplitude;

	return result;

}

unsigned int NoiseField::pseudorandom(const int x,const int y) const
{
	const unsigned int n = (SEED_X1 * x) +
		(SEED_Y1 * y);
	const unsigned int quad_term = SEED_Q2 * n * n +
		SEED_Q1 * n +
		SEED_Q0;
	return quad_term +
		(SEED_X2 * x) +
		(SEED_Y2 * y);
}
float NoiseField::fade(const float n) const
{
	float result;
	//result = (1 - cos(n * 3.14159265f)) * 0.5f;

	//result = -2 * (n*n*n) + 3 * (n*n);

	result = 6 * (n*n*n*n*n) - 15 * (n*n*n*n) + 10 * (n*n*n);

	//result = n;
	return result;
}


Vec2 NoiseField::lattice(int x, int y) const
{
	const unsigned int value = pseudorandom(x, y);
	const float radians = value * 6.28318530f;  // 2pi
	return { cos(radians), sin(radians) };
}

float NoiseField::perlinNoise(float x, float y)
{
	// calculate noise here
	const int x0 = int(floor(x / GRID_SIZE));
	const int y0 = int(floor(y / GRID_SIZE));

	float x_frac = x / GRID_SIZE - x0;
	float y_frac = y / GRID_SIZE - y0;

	const int x1 = x0 + 1;
	const int y1 = y0 + 1;
	const Vec2 lattice00 = lattice(x0, y0);
	const Vec2 lattice01 = lattice(x0, y1);
	const Vec2 lattice10 = lattice(x1, y0);
	const Vec2 lattice11 = lattice(x1, y1);

	const Vec2 direction00(-x_frac, -y_frac);
	const Vec2 direction01(-x_frac, 1.0f - y_frac);
	const Vec2 direction10(1.0f - x_frac, -y_frac);
	const Vec2 direction11(1.0f - x_frac, 1.0f - y_frac);

	const float value00 = dotProduct(lattice00, direction00);
	const float value01 = dotProduct(lattice01, direction01);
	const float value10 = dotProduct(lattice10, direction10);
	const float value11 = dotProduct(lattice11, direction11);

	const float x_fade1 = fade(x_frac);
	const float y_fade1 = fade(y_frac);
	const float x_fade0 = 1.0f - x_fade1;
	const float y_fade0 = 1.0f - y_fade1;

	const float value0 = value00 * y_fade0 + value01 * y_fade1;
	const float value1 = value10 * y_fade0 + value11 * y_fade1;

	const float value = value0 * x_fade0 + value1 * x_fade1;

	const float result = value * amplitude;
	return result;

}

float NoiseField::dotProduct(const Vec2& a, const Vec2& b) const
{
	return a.x * b.x + a.y * b.y;
}
