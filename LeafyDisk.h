#pragma once
#include "lib/ObjLibrary/ModelWithShader.h"
#include "Disk.h"

using namespace ObjLibrary;



class LeafyDisk : public Disk
{
private:
	const unsigned int HEIGHTMAP_SIZE = 32;
public:
	LeafyDisk(const ModelWithShader& model, Vector3 postion, float radius);
	~LeafyDisk() override = default;

	//Generates a hilly heightmap using a complex algorithm
	void generateHeightMap() override;
};
