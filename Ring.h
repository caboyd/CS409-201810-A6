#pragma once
#pragma once
#include "Entity.h"
#include "MovementGraph.h"

class World;

class Ring : public Entity
{
public:
	unsigned index;
	World const* world;
	MovementGraph* world_graph;
	
	const int pointValue = 1;
	const float velocity = 2.5f / 1000.0f;
	const float rotVelocity = 1.3f;
	const float radius = 0.7f;
	const float halfHeight = 0.1f;
	bool pickedUp;
	Vector3 targetPosition;

	std::deque<unsigned> path;
	unsigned curr_node_id;
	unsigned target_node_id;

	explicit Ring(unsigned i, const World& w, MovementGraph* mg, const ModelWithShader& model);;

	void update(double delta_time) override;
};

