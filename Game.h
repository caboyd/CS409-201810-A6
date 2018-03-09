#pragma once
#include "World.h"

class Game
{
	
private:
	World world;
	PickupManager pickup_manager;
	
	//Player player;


	//Initialize everything
	void init();


	//process keys and update game
	void update(float fixed_delta_time);


	void updateAnimations(float delta_time);

	
};
