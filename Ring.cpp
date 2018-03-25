#include "Ring.h"
#include "World.h"
#include "Collision.h"
#include "MovementGraph.h"
#include "Random.h"
#include "MathHelper.h"


Ring::Ring(unsigned i, World* w, MovementGraph* mg, ModelWithShader* model) : Entity(model)
{
	index = i;
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
	curr_node_id = Random::randu(world_graph->getNodeList().size() - 1);;
	target_node_id = curr_node_id;
	targetPosition = world_graph->getNodeList()[target_node_id].position;
	position = targetPosition;
	this->position.y += 0.1f;
}

inline void Ring::update(double delta_time)
{
	Vector3 direction = targetPosition - position;
	direction.normalizeSafe();

	const float speed_factor = world->getSpeedFactorAtPosition(float(position.x), float(position.z));
	const float distance = velocity * float(delta_time) * speed_factor;

	if (world_graph->getNodeList()[curr_node_id].disk_id == world_graph->getNodeList()[target_node_id].disk_id)
	{
		//Arc rotate around disk

		double old_dist = position.getDistance(targetPosition);
		Vector3 old_pos = position;
		const Vector3 center = world->disks[world_graph->getNodeList()[curr_node_id].disk_id]->position;
		Vector3 center_to_ring = position - center;
		//Vector3 dir_center_to_target = targetPosition - center;
		const Vector3 dir_pos_to_target = targetPosition - position;
		const double tangent_angle = center_to_ring.getRotatedY(MathHelper::M_PI_2).getAngle(dir_pos_to_target);


		//The radius to rotate around the the disk radius - 0.7;
		const float rad = world->disks[world_graph->getNodeList()[curr_node_id].disk_id]->radius - 0.7f;

		//The angle to rotate using arc length formula
		const float angle = distance / rad;

		//Rotate around the disk in the shorter direction
		if (tangent_angle < MathHelper::M_PI_2)
			center_to_ring.rotateY(angle);
		else
			center_to_ring.rotateY(-angle);


		position = center + center_to_ring;

		//To Make sure we don't drift off the radius
		position = center + center_to_ring.getNormalized() * rad;

	} else
	{
		//Move direct to another disk

		position += direction * distance;
	}

	//Rotate the ring's forward based on how much we moved
	forward.rotateY(rotVelocity * distance);

	//Set the rings height based on the world height at the position
	position.y = world->getHeightAtCirclePosition(float(position.x), float(position.z), radius) + 0.1f;

	//If on center of disk get new target
	if (Collision::pointCircleIntersection(float(targetPosition.x), float(targetPosition.z), float(position.x), float(position.z), 0.1f))
	{
		curr_node_id = target_node_id;
		if (path.empty())
		{
			//Get a random node that is not this same node
			unsigned rand = Random::randu(world_graph->getNodeList().size() - 1);
			while (rand == curr_node_id) rand = Random::randu(world_graph->getNodeList().size() - 1);


			//Remembers Ring 0s search data for display later
			if (index == 0)
			{
				//Perform all the searches so we can record the visits required for all
				path = world_graph->dijkstraSearch(curr_node_id, rand);
				path = world_graph->aStarSearch(curr_node_id, rand);
				path = world_graph->mmSearch(curr_node_id, rand);
				world_graph->memorizeLastSearch();
			} else
			{
				//Get the path between the nodes
				path = world_graph->mmSearch(curr_node_id, rand);
			}
			//Pop the node off the front and set it as the target
		}
		target_node_id = path.front();
		path.pop_front();
		targetPosition = world_graph->getNodeList()[target_node_id].position;

	}
}
