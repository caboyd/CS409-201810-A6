#pragma once
#include "lib/ObjLibrary/ModelWithShader.h"
#include "Disk.h"

using namespace ObjLibrary;



class SandyDisk : public Disk
{
private:
	const unsigned int HEIGHTMAP_SIZE = 64;
public:
	SandyDisk(const ModelWithShader& model, Vector3 postion, float radius);
	~SandyDisk() override = default;

	//Generates a roundish height map using the NoiseField's perlin noise
	void generateHeightMap() override;
};
