#pragma once
#include "CoordinateSystem.h"
#include "World.h"
#include "ShadowBox.h"
#include "Player.h"
#include "MovementGraph.h"
#include "LineRenderer.h"
#include "Bat.h"

/**
 *	Overarching Game class
 *	Has state of the game
 *	Controls all the objects in the game, updates them and draws them
 *
 */
class Game
{

private:
	const std::string ROD_FILENAME = "assets/Models/Rod.obj";
	const std::string RING_FILENAME = "assets/Models/Ring.obj";
	const std::string SKYBOX_FILENAME = "assets/Models/Skybox.obj";
	const std::string BAT_FILENAME = "assets/Models/BrownBat.obj";
	const std::string WORLD_FOLDER = "assets/Worlds/";

	//Game object models for drawing
	ModelWithShader skybox_model;
	ModelWithShader rod_model;
	ModelWithShader ring_model;
	ModelWithShader bat_model;

	//Game Objects
	World world;
	MovementGraph world_graph;
	Player player;
	PickupManager pickup_manager;
	std::vector<Bat> bats;

	//Camerars for behind the player and overview
	CoordinateSystem player_camera;
	CoordinateSystem overview_camera;
	CoordinateSystem* active_camera = &player_camera;

	//For Rotating around player with the camera
	bool camera_float = false;

	//For Swapping between Levels
	//Current level
	unsigned int level = 1;
	//list of level file names to load
	std::vector<std::string> levels;

	//Holds the movement graph's line data for faster drawing
	//of the movement graph lines
	std::vector<LineRenderer::Point> world_graph_point_line;

	//Used for Lighting and Shadows
	//The shadow box that updates the view area of the shadow maps
	ShadowBox shadow_box;
	//The view matrix of the light source that is used for shadows
	glm::mat4x4 light_view_matrix;

	glm::vec4* shadow_box_points;

public:
	//Empty Constructor, Init must be used to initialize game
	Game() = default;

	void initWorldGraphPointLine();
	void initBats();
	//Initialize The models, player,world, pickup manager, shadow box
	//lighting and camera.
	void init();

	//Update the game based on inputs using fixed delta time
	//Updates player and pickups
	void update(double fixed_delta_time);

	//Updates animations for the player using variable delta time
	void updateAnimations(double delta_time);

	//Displays weighted lines with different colors between each
	//node in the movement graph
	void displayMovementGraph(const glm::mat4x4& view_matrix,
		const glm::mat4x4& projection_matrix) const;

	//Render the game
	//Draws everything to the depth texture
	//Then uses Depth texture to draw everything with shadows to screen
	//Draws the UI
	void display();

	void displayBats(const glm::mat4x4& view_matrix,
		const glm::mat4x4& projection_matrix, const glm::vec3& camera_position);

	void displayBatsToDepthTexture(const glm::mat4& depth_view_projection_matrix);

	//Destory the current world and loads the next one from the worlds folder
	//Rebuilds the movement graph and world and resets player position and score
	void destroyIntoNextWorld();
	void destroyBats();

private:
	//Draws a line btween each node on the path for ring 0
	void displayRingZeroPath(const glm::mat4& view_matrix, const glm::mat4x4& projection_matrix) const;

	//Draws the id number of the node as text above each node for debugging purposes
	void displayNodeNameplates(const glm::mat4& view_matrix, const glm::mat4x4& projection_matrix) const;

	//Draws the Spheres with fading colors based on the search data obtained from the movement graph searches
	void displaySearchPathSpheres(const glm::mat4& view_matrix, const glm::mat4x4& projection_matrix) const;

	//Render game to depth texture for shadow mapping
	void renderToDepthTexture(glm::mat4& depth_vp);

	//Player functions to move the player
	void playerAccelerateForward(float delta_time);
	void playerAccelerateBackward(float delta_time);
	void playerAccelerateLeft(float delta_time);
	void playerAccelerateRight(float delta_time);
	void playerTurnLeft(float delta_time);
	void playerTurnRight(float delta_time);




};






