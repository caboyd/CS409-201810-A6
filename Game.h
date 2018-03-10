#pragma once
#include "CoordinateSystem.h"
#include "World.h"
#include "ShadowBox.h"
#include "Player.h"




class Game
{

private:
	World world;
	PickupManager pickup_manager;

	glm::mat4x4 light_view_matrix;
	ModelWithShader skybox_model;


	CoordinateSystem player_camera;
	CoordinateSystem overview_camera;
	CoordinateSystem* active_camera = &player_camera;

	Player player;
	//Set this to true to float the camera when the game starts
	bool camera_float = false;

	//Current level
	unsigned int level = 0;
	//list of level file names to load
	std::vector<std::string> levels;

	std::string world_folder;
public:
	ShadowBox shadow_box;

	Game() = default;

	//Initialize everything
	void init();


	//process keys and update game
	void update(double fixed_delta_time);


	void updateAnimations(double delta_time);

	void display();


	void destroyIntoNextWorld();

private:
	void renderToDepthTexture(glm::mat4& depth_vp);

};






