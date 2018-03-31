#pragma once
#include "Entity.h"

class Rod : public Entity
{
public:
	unsigned int pointValue;
	bool pickedUp;
	float radius;
	float halfHeight;

	Rod(const ModelWithShader& model, Vector3 position, unsigned int value);

	void update(double delta_time) override;
};

inline Rod::Rod(const ModelWithShader& model, Vector3 position, unsigned value): Entity(model, Vector3(position.x, position.y + 1.0f, position.z)), pointValue(value)
{
	pickedUp = false;
	radius = 0.1f;
	halfHeight = 1.0f;
}

inline void Rod::update(double delta_time)
{

}
