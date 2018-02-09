#pragma once
#include "lib/ObjLibrary/ModelWithShader.h"
#include "Disk.h"

using namespace ObjLibrary;



class RedRockDisk: public Disk
{
private:
	const unsigned int HEIGHTMAP_SIZE = 16;
public:
	RedRockDisk(ModelWithShader* model, Vector3 postion, float radius);
	~RedRockDisk() override = default;

	//Generates a pyramid style heightmap
	void generateHeightMap() override;
};
