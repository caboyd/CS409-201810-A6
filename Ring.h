#pragma once
#pragma once
#include "Entity.h"

class World;

class Ring : public Entity
{
public:
	unsigned int pointValue;
	float velocity;
	float rotVelocity;
	float radius;
	float halfHeight;
	bool pickedUp;
	Vector3 targetPosition;
	World *world;

	Ring(World* w, ModelWithShader* model, Vector3 pos);;

	void update(double delta_time) override;
};

