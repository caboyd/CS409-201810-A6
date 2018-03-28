#include "SandyDisk.h"
#include "MathHelper.h"
#include "NoiseField.h"
#include "PerformanceCounter.h"
#include "Random.h"

using namespace MathHelper;


SandyDisk::SandyDisk(ModelWithShader* model, Vector3 postion, float radius)
{
	this->model = model;
	this->position = postion;
	this->radius = radius;
	this->type = SANDY;
	this->heightMapSize = HEIGHTMAP_SIZE;

	//PerformanceCounter p;
	//p.start();
	SandyDisk::generateHeightMap();
	//std::cout << "sandy disk time " << p.getCounter() << std::endl;
	//Disk::generateHeightMapModel();
}


void SandyDisk::generateHeightMap()
{
	NoiseField ns(16.0f, 8.0f, Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF),
		Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF));

	//Do these outside of the loops so its O(n) instead of O(n^2)
	//Its 33x faster
	float* xz_6 = new float[heightMapSize + 1];
	const float pow_divisor = (1.0f - pow(0.5f, 6));
	for (unsigned int x = 0; x <= heightMapSize; x++)
	{
		const float ux = float(x) / heightMapSize;
		xz_6[x] = max(pow(ux, 6), pow(1.0f - ux, 6));
	}

	heightMap.resize(heightMapSize + 1, std::vector<float>(heightMapSize + 1));

	for (unsigned int x = 0; x <= heightMapSize; x++)
	{
		for (unsigned int z = 0; z <= heightMapSize; z++)
		{
			const float h = ns.perlinNoise(float(z), float(x));

			const float f = (1 - max(xz_6[x], xz_6[z])) / pow_divisor;

			heightMap[x][z] = h * f;

		}
	}

	delete[] xz_6;
}