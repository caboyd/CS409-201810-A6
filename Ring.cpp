#include "Ring.h"
#include "World.h"
#include "Collision.h"



Ring::Ring(World* w, ModelWithShader* model, Vector3 pos): Entity(model, pos)
{
	scalar = {1, 1, 1};
	forward = {0, 0, -1};
	pointValue = 1;
	pickedUp = false;
	world = w;
	radius = 0.7f;
	halfHeight = 0.1f;
	velocity = 2.5f / 1000.0f;
	rotVelocity = 1.3f;
	targetPosition = pos;
	this->position.y += 0.1f;
}

inline void Ring::update(double delta_time)
{
	Vector3 direction = targetPosition - position;
	direction.normalizeSafe();

	const float speed_factor = world->getSpeedFactorAtPosition(float(position.x), float(position.z));
	const float distance = velocity * float(delta_time) * speed_factor;
	position += direction * distance;
	float rot = rotVelocity * distance;
	forward.rotateY(rot);

	position.y = world->getHeightAtCirclePosition(float(position.x),float(position.z), radius) + 0.1f;

	//If on center of disk get new target
	if (Collision::pointCircleIntersection(float(targetPosition.x), float(targetPosition.z), float(position.x), float(position.z), radius))
	{
		targetPosition = world->getRandomDiskPosition();
	}
}
