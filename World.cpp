#include "World.h"
#include "fstream"
#include "lib/ObjLibrary/ObjStringParsing.h"
#include "lib/ObjLibrary/ObjModel.h"
#include "RedRockDisk.h"
#include "LeafyDisk.h"
#include "IcyDisk.h"
#include "SandyDisk.h"
#include "GreyRockDisk.h"
#include "MathHelper.h"
#include "PerformanceCounter.h"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "Random.h"
#include "LineRenderer.h"
#include "DepthTexture.h"
#include "Collision.h"

extern LineRenderer g_line_renderer;
extern DepthTexture g_depth_texture;

using namespace ObjLibrary;

World::~World()
{
	destroy();
}

void World::init(const std::string& filename)
{
	if (initialized) destroy();
	initialized = true;
	loadModels();


	std::ifstream input_file;

	input_file.open(filename.c_str(), std::ios::in);
	if (!input_file.is_open())
	{
		std::cerr << "Error: File \"" << filename << "\" does not exist" << std::endl;
		input_file.close();
		return;
	}

	std::string line;
	getline(input_file, line);
	if (!ObjStringParsing::startsWith(line, "DISK version 1"))
	{
		std::cerr << "Error: File \"" << filename << "\" is invalid" << std::endl;
		return;
	}

	//Get the world radus
	getline(input_file, line);
	this->worldRadius = float(atof(line.c_str()));

	//Get the number of disks and reserve memory for them
	getline(input_file, line);

	disks.reserve(atoi(line.c_str()) * sizeof(Disk*));

	//Read and Create Disks
	PerformanceCounter p{};
	p.start();
	while (getline(input_file, line))//While getting a line
	{

		line = ObjStringParsing::whitespaceToSpaces(line);

		size_t index = 0;

		//Chop spaces off the front of the line
		if (isspace(line[0]))
			index = ObjStringParsing::nextToken(line, 0);
		else
			index = 0;

		//Reads the x, z, and radius from the line
		const float y = 0.0f;
		const float x = float(atof(line.c_str() + index));
		index = ObjStringParsing::nextToken(line, index);
		const float z = float(atof(line.c_str() + index));
		index = ObjStringParsing::nextToken(line, index);
		const float radius = float(atof(line.c_str() + index));
		Vector3 pos(x, y, z);

		std::unique_ptr<Disk> disk;
		DiskType type = RED_ROCK;
		if (radius < 8)
		{
			type = RED_ROCK;
			disks.push_back(std::make_unique<RedRockDisk>(&this->RedRockModel, pos, radius));
		} else if (radius <= 12)
		{
			type = LEAFY;
			disks.push_back(std::make_unique<LeafyDisk>(&this->LeafyModel, pos, radius));
		} else if (radius <= 20)
		{
			type = ICY;
			disks.push_back(std::make_unique<IcyDisk>(&this->IcyModel, pos, radius));
		} else if (radius <= 30)
		{
			type = SANDY;
			disks.push_back(std::make_unique<SandyDisk>(&this->SandyModel, pos, radius));
		} else if (radius > 30)
		{
			type = GREY_ROCK;
			disks.push_back(std::make_unique<GreyRockDisk>(&this->GreyRockModel, pos, radius));
		}
		disksSorted[type].push_back(disks.back().get());
		pos.y = double(disks.back()->getHeightAtPosition(float(pos.x), float(pos.z)));



	}
	std::cout << "world creation time: " << p.getCounter() << "ms" << std::endl;

	std::cout << "Loaded file " << filename << std::endl;
}

void World::destroy()
{
	//reset the unique pointers to have them free the memory
	for (auto& ptr : disks)
	{
		ptr.reset();
	}
	disks.clear();
	disksSorted[0].clear();
	disksSorted[1].clear();
	disksSorted[2].clear();
	disksSorted[3].clear();
	disksSorted[4].clear();

	initialized = false;

}

void World::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix)
{

	draw(view_matrix, projection_matrix, glm::vec3(0, 0, 0));
}

void World::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos)
{

	//Call draw on each disk
	for (auto const& disk : disks)
	{
		disk->draw(view_matrix, projection_matrix, camera_pos);
	}


}

void World::drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix)
{
	drawOptimized(view_matrix, projection_matrix, glm::vec3(0, 0, 0));
}
void World::drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos)
{


	//For each disk draw all the black cylinder bases
//Setup to draw the black disk cylinders for all disks
	const MaterialForShader& base = disks[0]->model->getMaterial(1);

	const ObjShader::ShaderUniforms& uniforms = ObjShader::activateShader();
	base.activate(uniforms);

	glUniformMatrix4fv(uniforms.m_view_matrix, 1, false, &(view_matrix[0][0]));
	glUniform3fv(uniforms.m_camera_pos, 1, &(camera_pos.x));




	//Call draw on each black disk base
	for (auto const& disk : disks)
	{

		glm::mat4x4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(disk->position));
		model_matrix = glm::scale(model_matrix, glm::vec3(disk->radius, 1, disk->radius));
		glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;

		glUniformMatrix4fv(uniforms.m_model_matrix, 1, false, &(model_matrix[0][0]));
		glUniformMatrix4fv(uniforms.m_model_view_projection_matrix, 1, false, &(mvp_matrix[0][0]));
		disk->model->drawCurrentMaterial(1);
	}

	//For each disk draw the other stuff as groups
	for (int i = 0; i < 5; i++)
	{
		if (disksSorted[i].empty()) continue;
		const MaterialForShader& side = disksSorted[i][0]->model->getMaterial(0);
		const MaterialForShader& top = disksSorted[i][0]->model->getMaterial(2);

		side.activate(uniforms);
		for (auto const& disk : disksSorted[i])
		{
			glm::mat4x4 model_matrix = glm::mat4();
			model_matrix = glm::translate(model_matrix, glm::vec3(disk->position));
			model_matrix = glm::scale(model_matrix, glm::vec3(disk->radius, 1, disk->radius));
			glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;

			glUniformMatrix4fv(uniforms.m_model_matrix, 1, false, &(model_matrix[0][0]));
			glUniformMatrix4fv(uniforms.m_model_view_projection_matrix, 1, false, &(mvp_matrix[0][0]));
			disk->model->drawCurrentMaterial(0);

		}

		top.activate(uniforms);
		for (auto const& disk : disksSorted[i])
		{
			glm::mat4x4 model_matrix = glm::mat4();
			model_matrix = glm::translate(model_matrix, glm::vec3(disk->position));
			model_matrix = glm::scale(model_matrix, glm::vec3(disk->radius, 1, disk->radius));
			glm::mat4x4 mvp_matrix = projection_matrix * view_matrix *  model_matrix;

			glUniformMatrix4fv(uniforms.m_model_matrix, 1, false, &(model_matrix[0][0]));
			glUniformMatrix4fv(uniforms.m_model_view_projection_matrix, 1, false, &(mvp_matrix[0][0]));
			disk->model->drawCurrentMaterial(2);

			if (disk->heightMapModel.getMeshCountTotal() > 0)
			{
				model_matrix = glm::mat4();
				glm::vec3 pos = disk->position;
				const float corner = float(disk->radius * 0.70710678118);

				//Fix for overlapping faces causing flicker
				pos.y += 0.00001f;
				model_matrix = glm::translate(model_matrix, pos);
				model_matrix = glm::scale(model_matrix, glm::vec3(corner * 2 / disk->heightMapSize, 1, corner * 2 / disk->heightMapSize));
				mvp_matrix = projection_matrix * view_matrix *  model_matrix;

				glUniformMatrix4fv(uniforms.m_model_matrix, 1, false, &(model_matrix[0][0]));
				glUniformMatrix4fv(uniforms.m_model_view_projection_matrix, 1, false, &(mvp_matrix[0][0]));
				disk->heightMapModel.drawCurrentMaterial();
			}
		}

	}

}

void World::drawDepth(glm::mat4x4& depth_view_projection_matrix)
{
	//Call draw depth on each disk
	for (auto const& disk : disks)
	{
		disk->drawDepth(depth_view_projection_matrix);
	}

}

float World::getSpeedFactorAtPosition(float x, float z)const
{
	return getSpeedFactorAtPosition(x, z, 0);
}

float World::getSpeedFactorAtPosition(float x, float z, float r)const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::circleIntersection(x, z, r, float(disk->position.x), float(disk->position.z), disk->radius))
			return disk->getSpeedFactor();
	}
	//No collision with a disk
	return 1.0f;
}

float World::getAccelFactorAtPosition(float x, float z) const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::circleIntersection(x, z, 0, float(disk->position.x), float(disk->position.z), disk->radius))
			return disk->getAccelFactor();
	}
	//No collision with a disk
	return 1.0f;
}


float World::getFrictionAtPosition(float x, float z)const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::circleIntersection(x, z, 0, float(disk->position.x), float(disk->position.z), disk->radius))
			return disk->getFriction();
	}
	//No collision with a disk
	return 0.0001f;
}

float World::getSlopeFactorAtPosition(float x, float z)const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::circleIntersection(x, z, 0, float(disk->position.x), float(disk->position.z), disk->radius))
			return disk->getSlopeFactor();
	}
	//No collision with a disk
	return 0.0001f;
}

Vector3& World::getRandomDiskPosition()
{
	const unsigned int i = Random::randu(disks.size() - 1);
	return disks[i]->position;
}

bool World::isInitialized() const
{
	return initialized;
}

float World::getHeightAtPointPosition(const float x, const float z) const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::pointCircleIntersection(x, z, float(disk->position.x), float(disk->position.z), disk->radius))
			return disk->getHeightAtPosition(x, z);
	}
	//No collision with a disk
	return 0.0f;
}

float World::getHeightAtCirclePosition(const float x, const float z, const float r) const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::circleIntersection(x, z, r, float(disk->position.x), float(disk->position.z), disk->radius))
			return disk->getHeightAtPosition(x, z);
	}
	//No collision with a disk
	return -1000.0f;
}

bool World::isOnDisk(float x, float z) const
{
	return isOnDisk(x, z, 0);
}
bool World::isOnDisk(float x, float z, float r) const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::circleIntersection(x, z, r, float(disk->position.x), float(disk->position.z), disk->radius))
			return true;
	}
	return false;
}

bool World::isCylinderCollisionWithDisk(const Vector3& pos, float r, float half_height) const
{
	for (auto &disk : disks)
	{
		//If colliding with this disk return the height at the position on the disk
		if (Collision::circleIntersection(pos.x, pos.z, r, disk->position.x, disk->position.z, disk->radius))
		{
			float a = getHeightAtPointPosition(pos.x, pos.z) ;
			//If position is below height map it is inside
			if ( pos.y - half_height <= a )
			{
				return true;
			}
		}
			
	}
	return false;
}



void World::loadModels()
{
	//Load the model with shaders for each type
	ObjModel model;
	model.load(REDROCKMODEL_FILENAME);
	this->RedRockModel = model.getModelWithShader();
	model.load(LEAFYMODEL_FILENAME);
	this->LeafyModel = model.getModelWithShader();
	model.load(ICYMODEL_FILENAME);
	this->IcyModel = model.getModelWithShader();
	model.load(SANDYMODEL_FILENAME);
	this->SandyModel = model.getModelWithShader();
	model.load(GREYROCKMODEL_FILENAME);
	this->GreyRockModel = model.getModelWithShader();


}
