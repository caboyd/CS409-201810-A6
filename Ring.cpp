#include "Ring.h"
#include "World.h"
#include "Collision.h"
#include "MovementGraph.h"
#include "Random.h"


Ring::Ring(World* w, MovementGraph* mg, ModelWithShader* model) : Entity(model)
{
	scalar = { 1, 1, 1 };
	forward = { 0, 0, -1 };
	pointValue = 1;
	pickedUp = false;
	world = w;
	world_graph = mg;
	radius = 0.7f;
	halfHeight = 0.1f;
	velocity = 2.5f / 1000.0f;
	rotVelocity = 1.3f;
	curr_node_id = Random::randu(world_graph->node_list.size() - 1);;
	target_node_id = curr_node_id;
	targetPosition = world_graph->node_list[target_node_id].position;
	position = targetPosition;
	this->position.y += 0.1f;
}

inline void Ring::update(double delta_time)
{
	Vector3 direction = targetPosition - position;
	direction.normalizeSafe();

	const float speed_factor = world->getSpeedFactorAtPosition(float(position.x), float(position.z));
	const float distance = velocity * float(delta_time) * speed_factor;
	if(world_graph->node_list[curr_node_id].disk_id == world_graph->node_list[target_node_id].disk_id)
	{
		//Arc around disk
		Vector3 center = world->disks[world_graph->node_list[curr_node_id].disk_id]->position;
		Vector3 p = p - center;
		p.normalize();
		float radius = float(world_graph->node_list[target_node_id].position.getDistance(center));
		float ax = asin(p.x);
		float az = acos(p.z);
		float angle = distance * 0.001;
	
		Vector3 offset = Vector3(sin(ax + angle),0,cos(az + angle)) * radius;
		position = center + offset;

	}else
	{
		//Move direct
		position += direction * distance;
	}
	
	float rot = rotVelocity * distance;
	forward.rotateY(rot);

	position.y = world->getHeightAtCirclePosition(float(position.x), float(position.z), radius) + 0.1f;

	//If on center of disk get new target
	if (Collision::pointCircleIntersection(float(targetPosition.x), float(targetPosition.z), float(position.x), float(position.z), radius))
	{
		if (path.empty())
		{
			target_node_id = Random::randu(world_graph->node_list.size() - 1);
			path = world_graph->aStarSearch(curr_node_id, target_node_id);
			targetPosition = world_graph->node_list[target_node_id].position;
		} else
		{
			curr_node_id = target_node_id;
			target_node_id = path.front();
			path.pop_front();
			targetPosition = world_graph->node_list[target_node_id].position;
		}

	}
}
