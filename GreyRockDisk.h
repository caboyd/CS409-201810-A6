#pragma once
#include "lib/ObjLibrary/ModelWithShader.h"
#include "Disk.h"

using namespace ObjLibrary;

class GreyRockDisk : public Disk
{
private:
	const unsigned int HEIGHTMAP_SIZE = 80;
public:
	GreyRockDisk(const ModelWithShader& model, Vector3 postion, float radius);

	//Generates a rocky looking height map using the NoiseField's perlin noise
	void generateHeightMap() override;

};
