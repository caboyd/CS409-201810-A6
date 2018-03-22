#pragma once
#pragma once
#include "Entity.h"
#include "MovementGraph.h"

class World;

class Ring : public Entity
{
public:
	unsigned index;
	unsigned int pointValue;
	float velocity;
	float rotVelocity;
	float radius;
	float halfHeight;
	bool pickedUp;
	Vector3 targetPosition;
	World *world;
	MovementGraph* world_graph;
	std::deque<unsigned> path;
	unsigned curr_node_id;
	unsigned target_node_id;

	Ring(unsigned i, World* w, MovementGraph* mg, ModelWithShader* model);;

	void update(double delta_time) override;
};

