
#pragma once
#include <vector>
#include "Ring.h"
#include "Rod.h"



class PickupManager
{
private:
	ModelWithShader* rod_model;
	ModelWithShader* ring_model;

public:
	std::vector<Ring> rings;
private:
	std::vector<Rod> rods;

	World* world;
	MovementGraph* world_graph;
	unsigned int score;
public:
	PickupManager();

	void init(World* w, MovementGraph* mg, ModelWithShader* rod, ModelWithShader* ring);

	void update(const double delta_time);

	void checkForPickups(Vector3 player_position);

	int getScore()const;

	void addRod(Vector3 pos, unsigned int value);

	void addRing();

	void draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix) const ;
	void draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos) const;

	void drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix);
	void drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos);

	//Draw all of the disks to the depthRTT Shader
	//depthMatrixID is the uniform location for the DepthMVP matrix that will be calculated here
	void drawDepth(glm::mat4x4& depth_view_projection_matrix);

	void destroy();
};

