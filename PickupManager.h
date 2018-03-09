#pragma once
#include <vector>
#include "Ring.h"
#include "Rod.h"



class PickupManager
{
public:

	std::vector<Ring> rings;
	std::vector<Rod> rods;
	ModelWithShader* rodModel;
	ModelWithShader* ringModel;
	World* world;
	unsigned int score;

	PickupManager();

	void init(World* w, ModelWithShader* rod, ModelWithShader* ring);

	void update(const double delta_time);

	void checkForPickups(Vector3 player_position);

	void addRod(Vector3 pos, unsigned int value);

	void addRing(Vector3 pos);

	void destroy();
};

