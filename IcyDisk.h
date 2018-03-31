#pragma once
#include "lib/ObjLibrary/ModelWithShader.h"
#include "Disk.h"
#include "SIMD.h"

using namespace ObjLibrary;

class IcyDisk : public Disk
{
private:
	const unsigned int HEIGHTMAP_SIZE = 48;
public:
	IcyDisk(const ModelWithShader& model, Vector3 postion, float radius);
	~IcyDisk() override  = default ;

	//Generates a pointy heightmap using 200 points with random heights
	void generateHeightMap() override;
private:
	//Generates 200 random points
	void generatePoints(Vec3SOA points) const ;
};
