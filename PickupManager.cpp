#include "PickupManager.h"
#include "Collision.h"


PickupManager::PickupManager()
{}

void PickupManager::init(World* w, ModelWithShader* rod, ModelWithShader* ring)
{
	score = 0;
	world = w;
	rodModel = rod;
	ringModel = ring;
}

void PickupManager::update(const float delta_time)
{
	for (auto& ring : rings)
	{
		if(!ring.pickedUp)
			ring.update(delta_time);
	}
}

void PickupManager::checkForPickups(Vector3 player_position)
{
	for (auto& ring : rings)
	{
		if (!ring.pickedUp)
			if (cylinderIntersection(player_position, 0.25f, 0.8f, ring.position, ring.radius, ring.halfHeight))
			{
				ring.pickedUp = true;
				score += ring.pointValue;
			}
	}

	for (auto& rod : rods)
	{
		if (!rod.pickedUp)
			if (cylinderIntersection(player_position, 0.25f, 0.8f, rod.position, rod.radius, rod.halfHeight))
			{
				rod.pickedUp = true;
				score += rod.pointValue;
			}
	}
}

void PickupManager::addRod(Vector3 pos, unsigned value)
{
	rods.emplace_back(rodModel, pos, value);
}

void PickupManager::addRing(Vector3 pos)
{
	rings.emplace_back(world, ringModel, pos);
}

void PickupManager::destroy()
{
	rings.clear();
	rods.clear();
	score = 0;
}
