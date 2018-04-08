
#pragma once
#include <vector>
#include "lib/glm/glm.hpp"
#include "lib/ObjLibrary/ModelWithShader.h"
#include "Ring.h"
#include "Rod.h"

using ObjLibrary::ModelWithShader;

class World;
class MovementGraph;
class Player;


//Contains the rings and rods pickups
//Draws the rings and rods
//Updates the rings and rods


class PickupManager
{
private:
	ModelWithShader const* rod_model;
	ModelWithShader const* ring_model;

public:
	std::vector<Ring> rings;
private:
	std::vector<Rod> rods;

	World const* world;
	MovementGraph* world_graph;
	unsigned int score;
public:
	PickupManager();

	//Initialize all the rings and rods using data from the world and movementgraph
	void init(const World& w, MovementGraph& mg, const ModelWithShader& rod, const ModelWithShader& ring);

	//Updates every the position of each ring that hasnt been picked up
	//Uses data from the world
	void update(double delta_time);
	void pickupParticleExplosion(const glm::vec3& position);

	//Checks if a position has collided with a ring or rod to know if they have been picked
	//up and increments the score and marks them to no longer be drawn or updated
	void checkForPickups(const Player& player);

	int getScore()const;

	//Adds a new rod at a position with a score value
	void addRod(ObjLibrary::Vector3 pos, unsigned int score_value);

	//Adds a ring, rings are given random positions from the movement graph
	void addRing();

	//Draws the rings and rods regardless of the model chosen
	void draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix) const ;
	void draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos) const;


	//Draws the rings and rods optimally given the specific models by reducing material and mesh swaps
	void drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix);
	void drawOptimized(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix, const glm::vec3& camera_pos);

	//Draw all of the disks to the depthRTT Shader
	//depthMatrixID is the uniform location for the DepthMVP matrix that will be calculated here
	void drawDepth(const glm::mat4x4& depth_view_projection_matrix) const;

	//Draws the rings and rods that are only within a radius of a position (most likely player position)
	//If they are outside radius they will not be in the shadow map anyway so we don't need to draw them
	void drawDepthOptimized(const ObjLibrary::Vector3& position, float radius, const glm::mat4x4& depth_view_projection_matrix) const;

	//Destroys the vectors of rings and rods and reset score
	void destroy();
};

