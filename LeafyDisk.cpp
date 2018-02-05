#include "LeafyDisk.h"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "MathHelper.h"
#include "PerformanceCounter.h"
#include "Random.h"

using namespace MathHelper;

LeafyDisk::LeafyDisk(ModelWithShader* model, Vector3 postion, float radius)
{
	this->model = model;
	this->position = postion;
	this->radius = radius;
	this->type = LEAFY;
	this->heightMapSize = HEIGHTMAP_SIZE;

	generateHeightMap();
	generateHeightMapModel();
}



void LeafyDisk::generateHeightMap()
{

	const double LL = Random::randd(-1, 1);
	const double LC = Random::randd(-1, 1);
	const double LR = Random::randd(-1, 1);
	const double RI = Random::randd(-1, 1);
	const double RM = Random::randd(-1, 1);
	const double RO = Random::randd(-1, 1);

	int a1 = Random::randi(7);
	int a2 = Random::randi(7);
	const int ARM_COUNT = a1 < a2 ? a1 : a2;
	const double ARM_RADIANS = Random::randd(0, M_2PI);

	double AI = Random::randd(0, 1);
	double AM = Random::randd(0, 1);
	double AO = Random::randd(0, 1);
	if (Random::randd(0, 1) > 0.5)
	{
		AI = -AI;
		AM = -AM;
		AO = -AO;
	}

	heightMap.resize(heightMapSize + 1, std::vector<float>(heightMapSize + 1));

	for (unsigned int x = 0; x <= heightMapSize; x++)
	{
		for (unsigned int z = 0; z <= heightMapSize; z++)
		{
			//Convert x and z from 0 to heightMapSize to 0 to 1
			//ux is useable x, uy is useable y
			const double ux = (double)x / (double)heightMapSize;
			const double uy = (double)z / (double)heightMapSize;
			const double x2 = ux * 2.0 - 1.0;
			const double y2 = uy * 2.0 - 1.0;
			const double dist = min(sqrt(x2 * x2 + y2 * y2), 1);
			const double radians = atan2(y2, x2);

			const double ll = pow(ux, 4) * (1 - ux) * 12.0;
			const double lc = (cos(M_PI * x2) + 1) * 0.5;
			const double lr = pow((1 - ux), 4) * ux * 12.0;
			const double lsum = ll * LL + lc * LC + lr * LR;

			const double ri = pow(max(1 - dist, 0), 2);
			const double rm = sin(M_PI * min(pow(dist, 0.8) * (4 / 3), 1));
			const double ro = max(sin(M_PI * pow(dist, 1.6)), 0);
			const double rsum = ri * RI + rm * RM + ro * RO;

			const double ai = (sqrt(dist) - dist) * 4;
			const double am = pow(dist, 2) * pow((1 - dist), 2) * 16.0;
			const double ao = (sqrt(1 - dist) - (1 - dist)) * 4.0;
			const double asum = ai * AI + am * AM + ao * AO;

			const double non_arm_height = lsum * rsum;
			const double arm_radians = radians * ARM_COUNT + ARM_RADIANS;
			const double arm_magnitude = (sin(arm_radians) + 1.0) * 0.5;
			const double arm_height = arm_magnitude * asum;
			heightMap[x][z] = float(non_arm_height * 5.0 + arm_height * 3.0);

		}
	}
}

