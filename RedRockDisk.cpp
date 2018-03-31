#include "RedRockDisk.h"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/ObjLibrary/VertexDataFormat.h"
#include "MathHelper.h"
#include "PerformanceCounter.h"
#include "Random.h"


RedRockDisk::RedRockDisk(const ModelWithShader& model, Vector3 postion, float radius)
{
	this->model = &model;
	this->position = postion;
	this->radius = radius;
	this->type = RED_ROCK;
	this->heightMapSize = HEIGHTMAP_SIZE;

	generateHeightMap();
	generateHeightMapModel();
}

void RedRockDisk::generateHeightMap()
{
	std::vector<float> heights;
	heights.push_back(0.0f);
	heights.push_back(0.0f);
	for (unsigned int i = 2; i <= heightMapSize / 2; i++)
	{
		const float value = heights[i - 1];
		heights.push_back(value + Random::randf(-1, 2));
	}

	heightMap.resize(heightMapSize + 1, std::vector<float>(heightMapSize + 1));
	for (unsigned int x = 0; x <= heightMapSize; x++)
	{
		unsigned int peak_height_index = x > heightMapSize / 2 ? heightMapSize - x : x;
		for (unsigned int z = 0; z <= heightMapSize; z++)
		{
			if (z > heightMapSize - peak_height_index) peak_height_index--;
			unsigned int height_index = z;
			if (height_index > peak_height_index) height_index = peak_height_index;
			heightMap[x][z] = heights[height_index];
		}
	}
}

