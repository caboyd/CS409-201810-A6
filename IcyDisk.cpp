#include "IcyDisk.h"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "MathHelper.h"
#include "lib/ObjLibrary/Vector2.h"
#include "PerformanceCounter.h"
#include "SIMD.h"
#include "Random.h"

using namespace MathHelper;

IcyDisk::IcyDisk(ModelWithShader* model, Vector3 postion, float radius)
{
	this->model = model;
	this->position = postion;
	this->radius = radius;
	this->type = ICY;
	this->heightMapSize = HEIGHTMAP_SIZE;

	PerformanceCounter p{};
	p.start();
	IcyDisk::generateHeightMap();
	//std::cout << "icy disk time " << p.getCounter() << std::endl;
	Disk::generateHeightMapModel();
}

void IcyDisk::generatePoints(const Vec3SOA points) const
{
	//sse_data has 4 floats
	for (unsigned int i = 0; i < 200 / 4; i++)
	{	
		
		for (unsigned int j = 0; j < 4; j++)
		{
			const float h = float(heightMapSize) / 2.0f;
			float d1 = Random::randf(0, h);
			float d2 = Random::randf(0, h);
			float d3 = Random::randf(0, h);
			float d4 = Random::randf(0, h);

			const float d = max(max(d1, d2), max(d3, d4));

			const float a = Random::randf(0, float(M_2PI));

			const float x = h + cos(a) * d;
			const float z = h + sin(a) * d;

			const float m = (h - d) * 0.7f;

			const float y = Random::randf(-m, m);

			points.x[i].sse_data[j] = x;
			points.y[i].sse_data[j] = y;
			points.z[i].sse_data[j] = z;
		}
	}
}

void IcyDisk::generateHeightMap()
{
	//sse_t holds 4 floats
	sse_t a[600 / 4];

	//Allocate the point arrays on the stack beside eachother
	//x,y,z are arrays of 200 floats
	const Vec3SOA points{a + 0, a + 200/4, a + 400/4};

	//TODO this could be optimized
	generatePoints(points);

	sse_t distances[50];
	sse_t adds[50];
	sse_t subs[50];
	auto* dp = reinterpret_cast<float*>(&distances);
	auto* sp = reinterpret_cast<float*>(&subs);
	auto* ap = reinterpret_cast<float *>(&adds);


	heightMap.resize(heightMapSize + 1, std::vector<float>(heightMapSize + 1));
	for (unsigned int x = 0; x <= heightMapSize; x++)
	{
		for (unsigned int z = 0; z <= heightMapSize; z++)
		{
			//Optimized code is about 2-3x faster

			//Find the distance between x,z and the 200 points all at once using SIMD, which is fast
			vec2Distance1ToManySIMD(dp, float(x), float(z), 200, reinterpret_cast<float*>(points.x), reinterpret_cast<float*>(points.z));

			addAndSubSIMD(ap, sp, 200, reinterpret_cast<float*>(points.y), dp);
			const float p = max(maxSIMD(200, sp), 0);
			const float n = min(minSIMD(200, ap), 0);

			//OLD CODE that is slower

			//float min_h = 0;
			//float max_h = 0;
			//for (int i = 0; i < 200 / 4; i++)
			//{
			//	for (int k = 0; k < 4; k++)
			//	{
			//		const float d = distancef(x, z, points.x[i].sse_data[k], points.z[i].sse_data[k]);
			//		const float y = points.y[i].sse_data[k];

			//		float p = y - d;
			//		max_h = max(p, max_h);

			//		float n = y + d;
			//		min_h = min(n, min_h);
			//	}
			//}

			heightMap[x][z] = p + n;
		}
	}

}
