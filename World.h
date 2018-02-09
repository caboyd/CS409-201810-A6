#pragma once
#include "lib/ObjLibrary/ModelWithShader.h"
#include "ostream"
#include "Disk.h"
#include <vector>
#include "memory"

//The world class loads in all of the disks and is able to draw itself.
class World
{
public:
	//The Models we want to load
	const std::string REDROCKMODEL_FILENAME = "assets/Models/DiskA.obj";
	const std::string LEAFYMODEL_FILENAME = "assets/Models/DiskB.obj";
	const std::string ICYMODEL_FILENAME = "assets/Models/DiskC.obj";
	const std::string SANDYMODEL_FILENAME = "assets/Models/DiskD.obj";
	const std::string GREYROCKMODEL_FILENAME = "assets/Models/DiskE.obj";
	const std::string ROD_FILENAME = "assets/Models/Rod.obj";
	const std::string RING_FILENAME = "assets/Models/Ring.obj";

	//Each disk will have a pointer to one of these models
	//to save space by using the Flyweight pattern.
	ObjLibrary::ModelWithShader RedRockModel;
	ObjLibrary::ModelWithShader LeafyModel;
	ObjLibrary::ModelWithShader IcyModel;
	ObjLibrary::ModelWithShader SandyModel;
	ObjLibrary::ModelWithShader GreyRockModel;
	ObjLibrary::ModelWithShader RodModel;
	ObjLibrary::ModelWithShader RingModel;

	//Radius of the world
	float worldRadius = 0;

	//Vector of unique pointers to disks
	//unique pointer will manage the memory for us.
	std::vector<std::unique_ptr<Disk>> disks;

	std::vector<Disk*> disksSorted[5];
public:
	World() = default;
	~World();
	//Reads the given file and creates all of the disks 
	void init(const std::string& filename);
	//Clear memory
	void destroy();

	//Load the 5 models
	void loadModels();

	//Looks through all the disks in the world and if the position is on
	//a disk then get the height at the position on the disk
	float getHeightAtPointPosition(float x, float y);
	float getHeightAtCirclePosition(float x, float y, float r);
	//Draw all of the disks
	void draw(glm::mat4x4& view_matrix, glm::mat4x4& projection_matrix);

	//Draws the disks faster by grouping each disk by its materials
	//Draws all of the disks blacks bases first
	//For each disk type it draws all the disks sides
	//For each disk type it draws all the disks tops and height maps
	void drawOptimized(glm::mat4x4& view_matrix, glm::mat4x4& projection_matrix);
	void drawOptimized(glm::mat4x4& view_matrix, glm::mat4x4& projection_matrix, glm::vec3 camera_pos);
	//Draw all of the disks to the depthRTT Shader
	//depthMatrixID is the uniform location for the DepthMVP matrix that will be calculated here
	void drawDepth(unsigned int depth_matrix_id, glm::mat4x4& view_perspective_matrix);
};
