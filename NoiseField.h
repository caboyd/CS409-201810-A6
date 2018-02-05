#pragma once

struct Vec2
{
	float x;
	float y;

	Vec2(float X, float Y)
		: x(X), y(Y)
	{}
};

//NoiseField class to generate noise when given an x and y position.
//Should be instantiated with seeds, amplitude and grid_size
class NoiseField
{
private:
	float GRID_SIZE = 8.0;
	unsigned int SEED_X1 = 1273472206;
	unsigned int SEED_X2 = 4278162623;
	unsigned int SEED_Y1 = 1440014778;
	unsigned int SEED_Y2 = 524485263;
	unsigned int SEED_Q0 = 1498573726;
	unsigned int SEED_Q1 = 3476519523;
	unsigned int SEED_Q2 = 3905844518;
	float amplitude = 1.0;
public:
	NoiseField() = default;
	NoiseField(float grid_size, float a, unsigned x1, unsigned x2, unsigned y1, unsigned y2, unsigned q0, unsigned q1, unsigned q2);
	float valueNoise(float x, float y) const;
	float perlinNoise(float x, float y);

private:
	float fade(float n) const;
	unsigned int pseudorandom(int x, int y) const;
	Vec2 lattice(int x, int y) const;
	float dotProduct(const Vec2& a, const Vec2& b) const;
};