#pragma once
#include "Entity.h"

class Rod : public Entity
{
public:
	unsigned int pointValue;
	bool pickedUp;
	float radius;
	float halfHeight;

	Rod(ModelWithShader* model, Vector3 position, unsigned int value);;

	void update(float delta_time) override;
};

inline Rod::Rod(ModelWithShader* model, Vector3 position, unsigned value): Entity(model, position), pointValue(value)
{
	pickedUp = false;
	radius = 0.1f;
	halfHeight = 1.0f;
	this->position.y += 1.0f;
}

inline void Rod::update(float delta_time)
{

}
