#include "GreyRockDisk.h"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "MathHelper.h"
#include "NoiseField.h"
#include "PerformanceCounter.h"
#include "Random.h"

using namespace MathHelper;

GreyRockDisk::GreyRockDisk(const ModelWithShader& model, Vector3 postion, float radius)
{
	this->model = &model;
	this->position = postion;
	this->radius = radius;
	this->type = GREY_ROCK;
	this->heightMapSize = HEIGHTMAP_SIZE;

	generateHeightMap();
	generateHeightMapModel();
}

void GreyRockDisk::generateHeightMap()
{
	NoiseField ns[5] = { 
	{32.0f, 10.0f, Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), 
		Random::randu(0xFFFFFFFF),	Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF)} ,
	{ 16.0f, 7.0f, Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF),
		Random::randu(0xFFFFFFFF),	Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF) } ,
	{ 8.0f, 5.0f, Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF),
		Random::randu(0xFFFFFFFF),	Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF) } ,
	{ 4.0f, 3.5f, Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF),
		Random::randu(0xFFFFFFFF),	Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF) } ,
	{ 2.0f, 2.5f, Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF),
		Random::randu(0xFFFFFFFF),	Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF), Random::randu(0xFFFFFFFF) } };

	heightMap.resize(heightMapSize + 1, std::vector<float>(heightMapSize + 1));
	for (unsigned int x = 0; x <= heightMapSize; x++)
	{
		for (unsigned int z = 0; z <= heightMapSize; z++)
		{
			const float zf = (float)z;
			const float xf = (float)x;

			const float h = ns[0].perlinNoise(zf, xf) + ns[1].perlinNoise(zf, xf) + ns[2].perlinNoise(zf, xf) + ns[3].perlinNoise(zf, xf) + ns[4].perlinNoise(zf, xf);
			const float ux = (float)x / heightMapSize;
			const float uy = (float)z / heightMapSize;

			const float f = (1 - max(max(pow(ux, 6), pow(1.0f - ux, 6)), max(pow(uy, 6), pow(1.0f - uy, 6)))) / (1.0f - pow(0.5f, 6));
			heightMap[x][z] = h * f;
		}
	}
}
